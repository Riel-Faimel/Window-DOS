#pragma GCC optimize("Os")
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "ahci.hpp"
#include <lib32.hpp>

using namespace ATAdev;

InsideModuleRecord PDevDrv head {
    ProgIFIndex::PCI_AHCI,
    DeviceInfo::ClassType::storage,
    [](DeviceInfo *info) -> DeviceDriver *{
        return new ATAdev::SATA_control{*info};
    }
};

SATA_control::SATA_control(DeviceInfo &info):
cfg{static_cast<PCI_device_config *>(info.config)},
enumer{info.enumer}, cfg_ptr{info.config} {
    if (enumer) enumer->set_mod(cfg_ptr);

    const u32 bar = cfg->BAR[5] & ~0xFu;
    if ((cfg->BAR[5] & 0x1u) == 0 && bar) {
        ctrl_reg_base = reinterpret_cast<volatile void *>(bar);
    } else {
        ctrl_reg_base = nullptr;
        LocalDebugInfo(kout << "[AHCI] no BAR5 bar="; kout.hex(cfg->BAR[5]); kout << '\n';)
    }

    LocalDebugInfo(
    kout << "[AHCI] bar="; kout.hex((u16)(bar >> 16)); kout.hex((u16)bar);
    kout << " irq=" << (u16)cfg->Interrupt_line << '\n';
    )
}

bool SATA_control::init_controller() {
    if (!ctrl_reg_base) return false;

    auto &g = ghc();

    // 和 Linux 一样先做一次全局控制器复位（GHC.HR）。
    // BIOS 交接时 HBA 可能停在半初始化状态，只做 per-port COMRESET 不够。
    const u32 ghc_before = g.GHC;
    if (!(ghc_before & GHC_AE)) g.GHC = ghc_before | GHC_AE;

    const u32 saved_pi = g.PI;
    g.GHC |= 1u;   // HR
    // 规范允许复位最多 1 秒；io_wait 约 1us，这里给足 1M 次。
    // 正常情况下 HR 很快就自清，循环会立刻退出。
    for (u32 spin = 0; spin < 1000000u; ++spin) {
        if ((g.GHC & 1u) == 0) break;
        io_wait();
    }
    if (g.GHC & 1u) kout << "[AHCI] global reset stuck\n";

    g.GHC |= GHC_AE;
    g.PI = saved_pi;   // 复位后 PI 可能被清掉

    const u32 cap = g.CAP;
    port_count = static_cast<u8>((cap & 0x1Fu) + 1u);
    if (port_count > 32) port_count = 32;

    // 一屏内：ABAR / IRQ / 全局能力压成一行，物理机拍照先看这行。
    LocalDebugInfo(
    kout << "[AHCI] bar=";
    kout.hex(cfg->BAR[5] & ~0xFu);
    kout << " irq="; kout.hex(cfg->Interrupt_line);
    kout << " ghc="; kout.hex(g.GHC);
    kout << " cap="; kout.hex(cap);
    kout << " pi="; kout.hex((u16)g.PI);
    kout << " n="; kout.hex(port_count);
    kout << '\n';
    )

    // 一次分配 16 页连续物理内存做数据中转缓冲（恒等映射，可直接 DMA）。
    for (auto pkg : physicalpage.aloc(mem::PageNum{16}, true)) {
        if (pkg.pages < mem::PageNum{16}) break;
        bounce = reinterpret_cast<u32>(static_cast<void *>(pkg.base));
        bounce_size = 16u * 4096u;
        break;
    }
    if (!bounce) {
        LocalDebugInfo(kout << "[AHCI] bounce alloc failed\n";)
        return false;
    }
    LocalDebugInfo(
        kout << "[AHCI] CAP="; kout.hex((u16)cap);
        kout << " PI="; kout.hex((u16)g.PI);
        kout << " ports=" << (u16)port_count << '\n';
    )
    return true;
}

bool SATA_control::alloc_port(u8 id) {
    mem::Pointer<u8> base = 0;
    for (auto pkg : physicalpage.aloc(mem::PageNum{1}, true)) {
        if (pkg.pages < mem::PageNum{1}) break;
        base = pkg.base;
        break;
    }
    if (!base) return false;

    __builtin_memset(static_cast<void *>(base), 0, 4096);

    // 一页里放三段：命令表(1KB) / FIS(256B) / 命令表头(<=256B)
    ports[id].clb = static_cast<u32>(base);
    ports[id].fis = static_cast<u32>(base + 0x400u);
    ports[id].ct  = static_cast<u32>(base + 0x500u);
    return true;
}

namespace {
// COMRESET 的 DET 至少要拉低 1ms；用 0x80 端口做保守延时
// （每次 I/O 周期约 1us，现代芯片组上更快，所以给 20ms 的余量）。
void ahci_delay_ms(u32 ms) {
    for (u32 i = 0; i < ms * 1000u; i++) io_wait();
}
}

void SATA_control::init_port(u8 id) {
    auto &p = port(id);

    // 0. 先记链路状态：BIOS 可能已经把端口跑起来了，这时不该再 COMRESET。
    const u32 pre_det = p.SSTS & SSTS_DET_MASK;
    const u32 sig_before = p.SIG;
    const u32 sctl_before = p.SCHL;
    const u32 cmd_orig = p.CMD;

    // 链路没起来时，把端口的供电/起转位打开再复位。
    // BIOS 有时把 POD/SUD 留成 0，盘就一直没有被上电/起转。
    const bool need_reset = (pre_det != SSTS_DET_PRESENT);
    if (need_reset) {
        // 先把接口从 partial/slumber 拉回 active，再上电/起转。
        // ICH7 上如果 ICC 还停在低功耗态，后面的 COMRESET 不会建立链路。
        p.CMD = (p.CMD & ~PXCMD_ICC_MASK) | PXCMD_ICC_ACTIVE;
        p.CMD |= PXCMD_SUD | PXCMD_POD;
        ahci_delay_ms(20);
        LocalDebugInfo(
        kout << "[AHCI] p"; kout.hex(id);
        kout << " cmd="; kout.hex(cmd_orig);
        kout << " now="; kout.hex(p.CMD);
        kout << " sctl="; kout.hex(sctl_before);
        kout << " sig="; kout.hex(sig_before);
        kout << " ssts="; kout.hex(p.SSTS);
        kout << '\n';
        )
    }

    // 1. 停端口：ST 和 FRE 一起清；先等 CR，再等 FR。
    //    只清 ST 时如果 FRE 还在，CR 不会落下，后面的 CLB/FB 就写不进去。
    const u32 cmd_before = p.CMD;
    if (cmd_before & (PXCMD_ST | PXCMD_FRE)) {
        p.CMD = cmd_before & ~(PXCMD_ST | PXCMD_FRE);
        for (u32 spin = 0; spin < 0x40000u && (p.CMD & PXCMD_CR); ++spin) {}
        for (u32 spin = 0; spin < 0x40000u && (p.CMD & PXCMD_FR); ++spin) {}
    }
    // 物理机上 BIOS 常常把端口留在 FRE=1；只清 ST 时 CR 会一直是 1，
    // 后面写 CLB/FB 就全部落空。这行是区分“没找到设备”和“端口停不下来”的关键。
    if (p.CMD & PXCMD_CR) {
        LocalDebugInfo(
        kout << "[AHCI] p"; kout.hex(id);
        kout << " CR stuck cmd="; kout.hex(p.CMD);
        kout << " was="; kout.hex(cmd_orig); kout << '\n';
        )
    }

    // 2. COMRESET：只在链路本来就没起来时做。
    //    已经 link up 的端口直接重新编程即可，复位反而会把好链路打掉。
    if (need_reset) {
        p.SCHL = (p.SCHL & ~0x0Fu) | 0x01u;
        ahci_delay_ms(20);
        p.SCHL &= ~0x0Fu;
        for (u32 spin = 0; spin < 200000u; ++spin) {
            if ((p.SSTS & SSTS_DET_MASK) == SSTS_DET_PRESENT) break;
            io_wait();
        }
    }

    // 3. 先编程 CLB/FB，再开 FRE/ST；FRE=1 时 FB 是不能再改的。
    p.CMD_list_base_addr = ports[id].clb;
    p.CMD_list_base_addr_Upper_32_bits = 0;
    p.FIS_base_addr = ports[id].fis;
    p.FIS_base_addr_Upper_32_bits = 0;

    p.SERR = 0xFFFFFFFFu;
    p.IS = 0xFFFFFFFFu;
    p.CMD |= PXCMD_FRE;
    p.CMD |= PXCMD_ST;

    ports[id].present = true;
}

void SATA_control::fill_fis(CommandTable &ct, const ActionTable &at) const {
    FIS_to &f = ct.cfis;
    f = {};
    f.fis_type = 0x27;
    f.c_pmp = 0x80;   // C = 1
    f.command = static_cast<u8>(at.cmd);
    f.feature_l = static_cast<u8>(at.feature);
    f.feature_h = static_cast<u8>(at.feature >> 8);
    f.count_l = static_cast<u8>(at.LBA.count);
    f.count_h = static_cast<u8>(at.LBA.count >> 8);

    if (at.if_need_CHS) {
        f.device = static_cast<u8>(0xA0u | (at.head & 0x0Fu));
        f.lba0 = at.CHS.sector;
        f.lba1 = static_cast<u8>(at.CHS.cylinder);
        f.lba2 = static_cast<u8>(at.CHS.cylinder >> 8);
        return;
    }

    const bool identify =
        (at.cmd == ATAcmd::identify_ata) || (at.cmd == ATAcmd::identify_atapi);
    if (identify) {
        f.device = static_cast<u8>(0xA0);
    } else {
        // LBA 位；LBA28 时高 4 位也在这个寄存器的低半字节
        f.device = static_cast<u8>(0xE0);
        if (!at.need_48_mode) {
            f.device |= static_cast<u8>((at.LBA.low >> 24) & 0x0Fu);
        }
    }

    f.lba0 = static_cast<u8>(at.LBA.low);
    f.lba1 = static_cast<u8>(at.LBA.low >> 8);
    f.lba2 = static_cast<u8>(at.LBA.low >> 16);
    f.lba3 = static_cast<u8>(at.LBA.low >> 24);
    f.lba4 = static_cast<u8>(at.LBA.high);
    f.lba5 = static_cast<u8>(at.LBA.high >> 8);
}

DetectTable SATA_control::failed() const {
    DetectTable d{};
    d.status.busy = 1;
    return d;
}

DetectTable SATA_control::wait_done(u8 id) const {
    auto &p = port(id);

    bool timeout = true;
    for (u32 spin = 0; spin < 0x40000u; ++spin) {
        if ((p.CI & 1u) == 0) { timeout = false; break; }
    }

    const u32 tfd = p.TFD;
    DetectTable d{};
    d.status.buf = static_cast<u8>(tfd & 0xFFu);
    d.error = static_cast<ATAerror>((tfd >> 8) & 0xFFu);

    const u32 is = p.IS;
    const bool failed = (is & (PXIS_TFES | PXIS_TFERR | PXIS_D2HES | PXIS_OF)) != 0;
    if (failed) d.status.error = 1;
    if (is) p.IS = is;   // 写 1 清零
    LocalDebugInfo(
    if (timeout || failed) {
        // 链路没起来的空端口上，命令本来就不会完成；只记录链路已建立的失败。
        if ((p.SSTS & SSTS_DET_MASK) == SSTS_DET_PRESENT) {
            kout << "[AHCI] p"; kout.hex(id);
            kout << " cmd fail";
            if (timeout) kout << " timeout";
            kout << " tfd="; kout.hex(tfd);
            kout << " is="; kout.hex(is);
            kout << " ci="; kout.hex(p.CI);
            kout << '\n';
        }
    }
    )

    if (timeout) d.status.busy = 1;
    return d;
}

HotFunc DetectTable SATA_control::issue(
    u8 id, const ActionTable &at, void *dma, size_t bytes, bool write
) const {
    if (id >= 32 || !ports[id].present) return failed();

    auto &p = port(id);

    bool idle = false;
    for (u32 spin = 0; spin < 0x40000u; ++spin) {
        // 端口在跑的时候 CR 一直是 1，不能拿它当"空闲"判据；
        // 空闲 = 没有未完成的命令位、且设备不忙。
        if ((p.CI & 1u) == 0 && (p.TFD & 0x80u) == 0) {
            idle = true;
            break;
        }
    }
    if (!idle) {
        LocalDebugInfo(
        if ((p.SSTS & SSTS_DET_MASK) == SSTS_DET_PRESENT) {
            kout << "[AHCI] p"; kout.hex(id);
            kout << " busy ci="; kout.hex(p.CI);
            kout << " tfd="; kout.hex(p.TFD);
            kout << " cmd="; kout.hex(p.CMD);
            kout << '\n';
        }
        )
        return failed();
    }

    CommandHeader *ch = reinterpret_cast<CommandHeader *>(ports[id].clb);
    CommandTable *ct = reinterpret_cast<CommandTable *>(ports[id].ct);
    __builtin_memset(ch, 0, sizeof(CommandHeader));
    __builtin_memset(ct, 0, sizeof(CommandTable));

    fill_fis(*ct, at);

    u32 prdtl = 0;
    if (dma && bytes) {
        if (bytes > 4u * 1024u * 1024u) return failed();
        ct->prdt[0].dba = static_cast<u32>(reinterpret_cast<u32>(dma));
        ct->prdt[0].dbau = 0;
        ct->prdt[0].rsv = 0;
        ct->prdt[0].dbc = (static_cast<u32>(bytes) - 1u) & 0x3FFFFFu;
        prdtl = 1;
    }

    ch->ctba = ports[id].ct;
    ch->ctbau = 0;
    ch->dw0 = 5u | (write ? (1u << 6) : 0u) | (prdtl << 16);

    // 命令表写完之后才能敲 CI
    asm volatile ("" ::: "memory");

    p.CI = 1u;
    return wait_done(id);
}

DetectTable SATA_control::act(const ActionTable &at) const {
    const u8 id = at.device_index;
    if (id >= 32 || !ports[id].present) return failed();

    // 无数据相的命令直接下发；带数据的一律走 xfer()。
    return issue(id, at, nullptr, 0, false);
}

DetectTable SATA_control::detect(DeviceIndex device_index) const {
    const u8 id = static_cast<u8>(device_index);
    if (id >= 32 || !ports[id].present) return failed();

    const u32 tfd = port(id).TFD;
    DetectTable d{};
    d.status.buf = static_cast<u8>(tfd & 0xFFu);
    d.error = static_cast<ATAerror>((tfd >> 8) & 0xFFu);
    return d;
}

rtl::pair<DetectTable, size_t> SATA_control::read(
    const ActionTable &at, bufAddr buf, SectorCount count
) const {
    const u8 id = at.device_index;
    if (id >= 32 || !ports[id].present) return {failed(), 0};

    const size_t bytes = count.value() * 512u;
    if (!static_cast<void *>(buf) || bytes == 0 || bytes > bounce_size) {
        return {failed(), 0};
    }

    DetectTable d = issue(id, at, reinterpret_cast<void *>(bounce), bytes, false);
    if (d.status.error || d.status.busy) return {d, 0};

    __builtin_memcpy(static_cast<void *>(buf), reinterpret_cast<void *>(bounce), bytes);

    d.status.data_ready = 1;
    d.status.ready = 1;
    return {d, bytes};
}

rtl::pair<DetectTable, size_t> SATA_control::write(
    const ActionTable &at, bufAddr buf, SectorCount count
) const {
    const u8 id = at.device_index;
    if (id >= 32 || !ports[id].present) return {failed(), 0};

    const size_t bytes = count.value() * 512u;
    if (!static_cast<void *>(buf) || bytes == 0 || bytes > bounce_size) {
        return {failed(), 0};
    }

    __builtin_memcpy(reinterpret_cast<void *>(bounce), static_cast<void *>(buf), bytes);

    DetectTable d = issue(id, at, reinterpret_cast<void *>(bounce), bytes, true);
    if (d.status.error || d.status.busy) return {d, 0};

    d.status.data_ready = 1;
    d.status.ready = 1;
    return {d, bytes};
}

void SATA_control::work() {
    if (!init_controller()) {
        kout << "[AHCI] controller init failed\n";
        return;
    }

    // 全局复位之后重放一次 set_mod：ICH7 的 PCS(0x92) 端口时钟使能
    // 放在这里，确保它排在 GHC.HR 之后。
    if (enumer) enumer->set_mod(cfg_ptr);

    const u32 pi = ghc().PI;

    // 复位前的链路状态：3=BIOS 已经 link up，0=本来就没链路。
    // 物理机上如果 pre=03、post=00，就是复位把好链路打掉了。
    LocalDebugInfo(
        kout << "[AHCI] pre:";
        for (u8 id = 0; id < port_count; id++) {
            if ((pi & (1u << id)) == 0) continue;
            kout << ' '; kout.hex((u8)(port(id).SSTS & SSTS_DET_MASK));
        }
        kout << '\n';
    )

    // 先把所有已实现端口的命令表 / FIS / ST 准备好并做 COMRESET。
    // SSTS.DET 要到链路建立之后才有意义，所以探测放在下一轮。
    for (u8 id = 0; id < port_count; id++) {
        if ((pi & (1u << id)) == 0) continue;
        if (alloc_port(id)) init_port(id);
    }

    // 一行看完所有已实现端口的链路状态：3=link up，0=没设备。
    // 物理机如果整行都是 0，问题在端口/链路，不在 IDENTIFY。
    LocalDebugInfo(
        kout << "[AHCI] det:";
        for (u8 id = 0; id < port_count; id++) {
            if ((pi & (1u << id)) == 0) continue;
            kout << ' '; kout.hex((u8)(port(id).SSTS & SSTS_DET_MASK));
        }
        kout << '\n';
    )

    for (u8 id = 0; id < port_count; id++) {
        if ((pi & (1u << id)) == 0 || !ports[id].present) continue;

        const u32 ssts = port(id).SSTS;
        const u32 sig = port(id).SIG & 0xFFFFu;
        const u32 det = ssts & SSTS_DET_MASK;
        LocalDebugInfo(
        kout << "[AHCI] port " << (u16)id
             << " det=" << (u16)(ssts & SSTS_DET_MASK);

        // 只给真正链路上的端口打详细状态；空端口汇总在上面那行里。
        if (det == SSTS_DET_PRESENT) {
            auto &p = port(id);
            kout << "[AHCI] p"; kout.hex(id);
            kout << " cmd="; kout.hex(p.CMD);
            kout << " tfd="; kout.hex(p.TFD);
            kout << " is="; kout.hex(p.IS);
            kout << " serr="; kout.hex(p.SERR);
            kout << " sig="; kout.hex((u16)sig);
            kout << '\n';
        }
        )

        // 有 ATA 签名或链路已建立才值得发 IDENTIFY；空端口上会白等一次超时。
        if (sig != SIG_ATA && det != SSTS_DET_PRESENT) {
            ports[id].present = false;
            continue;
        }

        auto info = identify(DeviceIndex{id});
        // 空端口上命令可能根本没跑起来，IDENTIFY 缓冲全零；这种不算磁盘。
        if (info && !info->su_num.low && !info->su_num.high) {
            delete info;
            info = nullptr;
        }
        if (!info) {
            ports[id].present = false;
            // 链路真的起来了却 IDENTIFY 失败，才是值得记录的；空端口不刷屏。
            LocalDebugInfo(
            if (det == SSTS_DET_PRESENT) {
                auto d = detect(DeviceIndex{id});
                kout << "[AHCI] p"; kout.hex(id);
                kout << " identify failed tfd=";
                kout.hex((u8)d.status.buf);
                kout << " err="; kout.hex((u8)d.error); kout << '\n';
            }
            )
            continue;
        }

        info->device = id;
        kom->append(new SATA{*info, *this, DeviceIndex{id}});

        LocalDebugInfo(
        kout << "[AHCI] +disk p" << id
             << " mode=" << (u16)info->address_mode
             << " sect="; kout.hex(info->su_num.low); kout << '\n';
        )
        delete info;
    }
}

void SATA_control::off() {
    // 注销 work() 里发布的磁盘（目前没有优雅的卸载路径）
}


SATA::SATA(const PhysicalDiskInfo &diskinfo_, const SATA_control &ctl_, DeviceIndex port_):
diskinfo{diskinfo_}, ctl{ctl_}, port{port_} {}

void SATA::check() {
    // 端口在线状态由控制器在 work() 时确认；热插拔未支持。
}

HotFunc unsigned SATA::read(void *buf, unsigned lba, unsigned lbah, unsigned count) {
    if (!buf || !count) return (unsigned)-1;
    if (count > 0xFFFFu) count = 0xFFFFu;   // SectorCount 是 16 位

    auto [d, n] = ctl.read(
        bufAddr{buf}, LBA{lbah, lba}, SectorCount{static_cast<u16>(count)}, port
    );
    if (d.status.error || d.status.busy) return (unsigned)-1;
    return static_cast<unsigned>(n);
}

HotFunc unsigned SATA::write(void *buf, unsigned lba, unsigned lbah, unsigned count) {
    if (!buf || !count) return (unsigned)-1;
    if (count > 0xFFFFu) count = 0xFFFFu;

    auto [d, n] = ctl.write(
        bufAddr{buf}, LBA{lbah, lba}, SectorCount{static_cast<u16>(count)}, port
    );
    if (d.status.error || d.status.busy) return (unsigned)-1;
    return static_cast<unsigned>(n);
}

unsigned SATA::cmd(unsigned, String, void *, unsigned) {
    return 0;
}
