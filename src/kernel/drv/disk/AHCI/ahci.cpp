#include "ahci.hpp"

import lib32;

using namespace ATAdev;

InsideModuleRecord PDevDrv head {
    ProgIFIndex::PCI_AHCI,
    DeviceInfo::ClassType::storage,
    [](DeviceInfo *info) -> DeviceDriver *{
        return new ATAdev::SATA_control{*info};
    }
};

SATA_control::SATA_control(DeviceInfo &info):
cfg{*static_cast<PCI_device_config *>(info.config)} {
    if (info.enumer) info.enumer->set_mod(info.config);

    const u32 bar = cfg.BAR[5] & ~0xFu;
    if ((cfg.BAR[5] & 0x1u) == 0 && bar) {
        ctrl_reg_base = reinterpret_cast<volatile void *>(bar);
    } else {
        ctrl_reg_base = nullptr;
    }

    LocalDebugInfo(
        kout << "[AHCI] bar="; kout.hex((u16)(bar >> 16)); kout.hex((u16)bar);
        kout << " irq=" << (u16)cfg.Interrupt_line << '\n';
    )
}

bool SATA_control::init_controller() {
    if (!ctrl_reg_base) return false;

    auto &g = ghc();
    g.GHC |= GHC_AE;

    const u32 cap = g.CAP;
    port_count = static_cast<u8>((cap & 0x1Fu) + 1u);
    if (port_count > 32) port_count = 32;

    // 一次分配 16 页连续物理内存做数据中转缓冲（恒等映射，可直接 DMA）。
    for (auto pkg : physicalpage.aloc(mm::PageNum{16}, true)) {
        if (pkg.pages < mm::PageNum{16}) break;
        bounce = reinterpret_cast<u32>(mm::address_to_void_p(pkg.base));
        bounce_size = 16u * 4096u;
        break;
    }
    if (!bounce) return false;

    LocalDebugInfo(
        kout << "[AHCI] CAP="; kout.hex((u16)cap);
        kout << " PI="; kout.hex((u16)g.PI);
        kout << " ports=" << (u16)port_count << '\n';
    )
    return true;
}

bool SATA_control::alloc_port(u8 id) {
    u32 base = 0;
    for (auto pkg : physicalpage.aloc(mm::PageNum{1}, true)) {
        if (pkg.pages < mm::PageNum{1}) break;
        base = reinterpret_cast<u32>(mm::address_to_void_p(pkg.base));
        break;
    }
    if (!base) return false;

    __builtin_memset(reinterpret_cast<void *>(base), 0, 4096);

    // 一页里放三段：命令表(1KB) / FIS(256B) / 命令表头(<=256B)
    ports[id].clb = base;
    ports[id].fis = base + 0x400u;
    ports[id].ct  = base + 0x500u;
    return true;
}

void SATA_control::init_port(u8 id) {
    auto &p = port(id);

    // 1. 停端口：先清 ST，再等 CR 落下
    for (u32 spin = 0; spin < 0x40000u && (p.CMD & PXCMD_CR); ++spin) {}
    if (p.CMD & PXCMD_ST) {
        p.CMD &= ~PXCMD_ST;
        for (u32 spin = 0; spin < 0x40000u && (p.CMD & PXCMD_CR); ++spin) {}
    }

    // 2. COMRESET：先建立链路。复位会把 CLB/FB 清掉，所以必须排在编程之前。
    p.SCHL = (p.SCHL & ~0x0Fu) | 0x01u;
    for (u32 spin = 0; spin < 0x10000u; ++spin) { asm volatile ("" ::: "memory"); }
    p.SCHL &= ~0x0Fu;
    for (u32 spin = 0; spin < 0x40000u; ++spin) {
        if ((p.SSTS & SSTS_DET_MASK) == SSTS_DET_PRESENT) break;
    }

    // 3. 起端口：FRE -> CLB/FB -> ST
    p.CMD |= PXCMD_FRE;
    p.CMD_list_base_addr = ports[id].clb;
    p.CMD_list_base_addr_Upper_32_bits = 0;
    p.FIS_base_addr = ports[id].fis;
    p.FIS_base_addr_Upper_32_bits = 0;

    p.SERR = 0xFFFFFFFFu;
    p.IS = 0xFFFFFFFFu;
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
    if (is & (PXIS_TFES | PXIS_TFERR | PXIS_D2HES | PXIS_OF)) d.status.error = 1;
    if (is) p.IS = is;   // 写 1 清零

    if (timeout) d.status.busy = 1;
    return d;
}

DetectTable SATA_control::issue(
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
    if (!idle) return failed();

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

    const u32 pi = ghc().PI;

    // 先把所有已实现端口的命令表 / FIS / ST 准备好并做 COMRESET。
    // SSTS.DET 要到链路建立之后才有意义，所以探测放在下一轮。
    for (u8 id = 0; id < port_count; id++) {
        if ((pi & (1u << id)) == 0) continue;
        if (alloc_port(id)) init_port(id);
    }

    for (u8 id = 0; id < port_count; id++) {
        if ((pi & (1u << id)) == 0 || !ports[id].present) continue;

        const u32 ssts = port(id).SSTS;
        const u32 sig = port(id).SIG & 0xFFFFu;

        LocalDebugInfo(
            kout << "[AHCI] port " << (u16)id
                 << " det=" << (u16)(ssts & SSTS_DET_MASK);
            kout << " sig="; kout.hex((u16)sig); kout << '\n';
        )

        // 有 ATA 签名或链路已建立才值得发 IDENTIFY；空端口上会白等一次超时。
        if (sig != SIG_ATA && (ssts & SSTS_DET_MASK) != SSTS_DET_PRESENT) {
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
            LocalDebugInfo(kout << "[AHCI] port " << (u16)id << " identify failed\n";)
            continue;
        }

        info->device = id;
        kom->append(new SATA{*info, *this, DeviceIndex{id}});

        LocalDebugInfo(
            kout << "[AHCI] +disk port=" << (u16)id
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

unsigned SATA::read(void *buf, unsigned lba, unsigned lbah, unsigned count) {
    if (!buf || !count) return (unsigned)-1;
    if (count > 0xFFFFu) count = 0xFFFFu;   // SectorCount 是 16 位

    auto [d, n] = ctl.read(
        bufAddr{buf}, LBA{lbah, lba}, SectorCount{static_cast<u16>(count)}, port
    );
    if (d.status.error || d.status.busy) return (unsigned)-1;
    return static_cast<unsigned>(n);
}

unsigned SATA::write(void *buf, unsigned lba, unsigned lbah, unsigned count) {
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
