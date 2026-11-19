#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ide.hpp"

InsideModuleRecord PDevDrv head {
    ProgIFIndex::PCI_IDE,
    DeviceInfo::ClassType::storage,
    [](DeviceInfo *) -> DeviceDriver *{ return new ATAdev::IDE_control{}; }
};

namespace {
void DiskInfoPrint(PhysicalDiskInfo *diskinfo) {
    kout << "[IDE] +disk ";
    if (diskinfo->address_mode & 0b10) {
        kout << "LBA";
        if (diskinfo->address_mode & 0b100) {
            kout << "48 ";
        } else {
            kout << "28 ";
        }
    } else if (diskinfo->address_mode & 1) {
        kout << "CHS ";
    } else {
        kout << "Bad ";
    }
    kout << ", ";
    if (diskinfo->PIO_supported) {
        kout << "PIO";
    } else {
        kout << "Unknown";
    }
    kout << " Mode\n";
}
} // namespace 

namespace ATAdev {

IDE_control::IDE_control() {
    interrupt_distributor.reg_irq(&when_PATA_Master_cut_handler, IRQ::ATA_Master);
    interrupt_distributor.reg_irq(&when_PATA_Slave_cut_handler, IRQ::ATA_Slave);
}

void IDE_control::work() {
    for (auto disk_ID : *this) {
        auto diskinfo = identify(disk_ID);
        if (diskinfo) {
            // disk exist
            DebugInfo(DiskInfoPrint(diskinfo);)
            auto disk = new IDE{*diskinfo, *this, disk_ID};
            kom->append(disk);
#undef LocalDebug
#include <global/macro.hpp>
            LocalDebugInfo(
                static bool a = false;
                kout << "enter" << a << '\n';
                if (a) {
                    u16 buf[256];
                    disk->read(buf, 0x80, 0, 1);
                    for (auto ch : buf) {
                        print_hex(ch, false);print_char(' ');
                    }
                    while(1);
                } else {
                    a = true;
                }
            )
            ASMLabelMark(resolve_done)
        } 
#undef LocalDebug
#include <global/macro.hpp>
    LocalDebugInfo(else {
            kout << "null port\n";
        })
    }
}

void IDE_control::off() {
    // delete LogicalDisk
}

__attribute__((optimize("O0")))
DetectTable IDE_control::act(const ActionTable &at) const {
    #define timeout_guard(...) \
    for (result = detect(DeviceIndex{at.device_index});__VA_ARGS__;timeout_counter++, result = detect(DeviceIndex{at.device_index})) {\
        if (timeout_counter > 0xFFF0) return result;\
    }
    // IDE
    auto [chan, device] = diskID2chan_dev(DeviceIndex{at.device_index});
    u16 timeout_counter = 0;
    DetectTable result;

    timeout_guard(result.status.busy)

    if (at.fill_addr){
        if (at.if_need_CHS) {
            // well.. 
            outb(static_cast<u8>(at.LBA.count), chan + CMDBlock::sector_num);
            outb(at.CHS.sector, chan + CMDBlock::sector_index);
            outb(static_cast<u8>(at.CHS.cylinder), chan + CMDBlock::cylinder_l);
            outb(static_cast<u8>(at.CHS.cylinder >> 8), chan + CMDBlock::cylinder_h);
            device |= at.head;
        } else {
            // LBA mode
            device |= 0b01000000;
            u8 lba28l = static_cast<u8>(at.LBA.low), lba28m = static_cast<u8>(at.LBA.low >> 8),
            lba28h = static_cast<u8>(at.LBA.low >> 16), lba48l = static_cast<u8>(at.LBA.low >> 24),
            lba28e = lba48l & 0x0Fu, lba48m = static_cast<u8>(at.LBA.high),
            lba48h = static_cast<u8>(at.LBA.high >> 8);

            if (at.need_48_mode) {
                outb(static_cast<u8>(at.feature >> 8), chan + CMDBlock::feature);
                outb(static_cast<u8>(at.LBA.count >> 8), chan + CMDBlock::sector_num);
                outb(lba48l, chan + CMDBlock::LBAl);
                outb(lba48m, chan + CMDBlock::LBAm);
                outb(lba48h, chan + CMDBlock::LBAh);
            } else {
                device |= lba28e;
            }

            outb(static_cast<u8>(at.feature), chan + CMDBlock::feature);
            outb(static_cast<u8>(at.LBA.count), chan + CMDBlock::sector_num);
            outb(lba28l, chan + CMDBlock::LBAl);
            outb(lba28m, chan + CMDBlock::LBAm);
            outb(lba28h, chan + CMDBlock::LBAh);
        }

        timeout_guard(!result.status.ready || result.status.busy)
    }

    if (at.fill_device) {
        outb(device, chan + CMDBlock::device);
        timeout_guard(!result.status.ready || result.status.busy)
    }

    if (at.send_cmd) {
        outb(static_cast<u8>(at.cmd), chan + CMDBlock::CMD);
        timeout_guard(!result.status.ready || result.status.busy || result.status.error)
    }

    return result;
}

__attribute__((optimize("O0")))
DetectTable IDE_control::detect(DeviceIndex device_index) const {
    auto [chan, _] = diskID2chan_dev(device_index);
    return {
        .error = (ATAerror)inb(chan + CMDBlock::error),
        .status = (ATAstatus)inb(chan + CMDBlock::status),
    };
}

rtl::pair<DetectTable, size_t> IDE_control::read(
    const ActionTable &at, bufAddr buf, SectorCount count
) const {
    const DeviceIndex device{static_cast<u8>(at.device_index)};

    DetectTable d = act(at);
    if (d.status.busy || d.status.error) return {d, 0};

    size_t bytes_read = 0;
    size_t timeout_count = 0;
    auto [chan, _] = diskID2chan_dev(device);

    for (auto sec : rtl::range{count}) {
        for (;d.status.busy || !d.status.data_ready;
            timeout_count++, d = detect(device)) {
            if (timeout_count > 512) return {d, 0};
        }
        for (size_t i = 0;i < 256;i++) {
            buf[i + static_cast<byteCount>(sec).to_size_t() / 2] = inw(chan + CMDBlock::data);
            bytes_read+=2;
        }
    }
    return {d, bytes_read};
}

rtl::pair<DetectTable, size_t> IDE_control::write(
    const ActionTable &at, bufAddr buf, SectorCount count
) const {
    const DeviceIndex device{static_cast<u8>(at.device_index)};

    DetectTable d = act(at);
    if (d.status.busy || d.status.error) return {d, 0};

    size_t bytes_write = 0;
    size_t timeout_count = 0;
    auto [chan, _] = diskID2chan_dev(device);

    for (auto sec : rtl::range{count}) {
        for (;d.status.busy || !d.status.data_ready;
            timeout_count++, d = detect(device)) {
            if (timeout_count > 512) return {d, 0};
        }
        for (size_t i = 0;i < 256;i++) {
            outw(buf[i + static_cast<byteCount>(sec).to_size_t() / 2], chan + CMDBlock::data);
            bytes_write+=2;
        }
    }
    return {d, bytes_write};
}


IDE::IDE(const PhysicalDiskInfo &diskinfo_, const IDE_control &ctl_, DeviceIndex device_):
diskinfo{diskinfo_}, ctl{ctl_}, device{device_} {}

unsigned IDE::read(void *buf, unsigned lba, unsigned lbah, unsigned count) {
    if (!buf || !count) return (unsigned)-1;
    if (count > 0xFFFFu) count = 0xFFFFu;   // SectorCount 是 16 位

    if (diskinfo.address_mode & 0b10) {
        auto [d, n] = ctl.read(
            bufAddr{buf}, LBA{lbah, lba}, SectorCount{static_cast<u16>(count)}, device
        );
        if (d.status.error || d.status.busy) return (unsigned)-1;
        return static_cast<unsigned>(n);
    }

    if (!(diskinfo.address_mode & 1)) return (unsigned)-1;

    const u16 spt = diskinfo.CHS_sectors_per_track;
    const u8 heads = static_cast<u8>(diskinfo.CHS_heads);
    if (!spt || !heads) return (unsigned)-1;

    // CHS 的越界由这里夹：基类只有每道扇区数和磁头数，不知道有几柱面。
    const u32 per_cyl = static_cast<u32>(spt) * heads;
    const u32 total = static_cast<u32>(diskinfo.CHS_cylinders) * per_cyl;
    if (lba >= total) return (unsigned)-1;
    if (count > total - lba) count = total - lba;

    auto [d, n] = ctl.read(
        bufAddr{buf}, CHS{LBA{lbah, lba}, spt, heads},
        SectorCount{static_cast<u16>(count)}, device
    );
    if (d.status.error || d.status.busy) return (unsigned)-1;
    return static_cast<unsigned>(n);
}

unsigned IDE::write(void *buf, unsigned lba, unsigned lbah, unsigned count) {
    if (!buf || !count) return (unsigned)-1;
    if (count > 0xFFFFu) count = 0xFFFFu;

    if (diskinfo.address_mode & 0b10) {
        auto [d, n] = ctl.write(
            bufAddr{buf}, LBA{lbah, lba}, SectorCount{static_cast<u16>(count)}, device
        );
        if (d.status.error || d.status.busy) return (unsigned)-1;
        return static_cast<unsigned>(n);
    }

    if (!(diskinfo.address_mode & 1)) return (unsigned)-1;

    const u16 spt = diskinfo.CHS_sectors_per_track;
    const u8 heads = static_cast<u8>(diskinfo.CHS_heads);
    if (!spt || !heads) return (unsigned)-1;

    const u32 per_cyl = static_cast<u32>(spt) * heads;
    const u32 total = static_cast<u32>(diskinfo.CHS_cylinders) * per_cyl;
    if (lba >= total) return (unsigned)-1;
    if (count > total - lba) count = total - lba;

    auto [d, n] = ctl.write(
        bufAddr{buf}, CHS{LBA{lbah, lba}, spt, heads},
        SectorCount{static_cast<u16>(count)}, device
    );
    if (d.status.error || d.status.busy) return (unsigned)-1;
    return static_cast<unsigned>(n);
}

unsigned IDE::cmd(unsigned, String, void *, unsigned) {
    return 0;
}

rtl::pair<u16, u8> IDE_control::diskID2chan_dev(DeviceIndex device_index) {
    // mm, ms, sm, ss
    return {
        (static_cast<u8>(device_index) & 2u)?(u16)0x170:(u16)0x1F0,
        (static_cast<u8>(device_index) & 1u)?(u8)0b10110000:(u8)0b10100000
    };
}
    
} // namespace ATAdev 