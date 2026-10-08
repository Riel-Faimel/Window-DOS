//#pragma GCC optimize("Os")
#include "_ide.hpp"

void *init_IDE_controller(DeviceConfig &) {
    return new IDE_control{};
}

static void DiskInfoPrint(DiskInfo *diskinfo) {
    cout << "[INFO] ";
    if (diskinfo->address_mode & 0b10) {
        cout << "LBA";
        if (diskinfo->address_mode & 0b100) {
            cout << "48 ";
        } else {
            cout << "28 ";
        }
    } else if (diskinfo->address_mode & 1) {
        cout << "CHS ";
    } else {
        cout << "Bad ";
    }
    cout << "Disk Got, ";
    if (diskinfo->PIO_supported) {
        cout << "PIO";
    } else {
        cout << "Unknown";
    }
    cout << " Mode\n";
}

IDE_control::IDE_control() {
    interrupt_distributor.reg_irq(&when_PATA_Master_cut_handler, IRQ::ATA_Master);
    interrupt_distributor.reg_irq(&when_PATA_Slave_cut_handler, IRQ::ATA_Slave);
    
    for (auto disk_ID : *this) {
        auto diskinfo = identify(disk_ID);
        if (diskinfo) {
            // disk exist
            DebugInfo(DiskInfoPrint(diskinfo);)
            auto disk = new IDE{*diskinfo, *this, disk_ID};
#undef LocalDebug
#include <global/macro.hpp>
            LocalDebugInfo(
                static bool a = false;
                cout << "enter" << a << '\n';
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
            partmanager->resolve(disk);
            ASMLabelMark(resolve_done)
        } 
#undef LocalDebug
#include <global/macro.hpp>
    LocalDebugInfo(else {
            cout << "null port\n";
        })
    }
}

__attribute__((optimize("O0")))
DetectTable IDE_control::act(const ActionTable &at) const {
    #define _LM_act_timeout_guard(...) for (result = detect(DeviceIndex{at.device_index});__VA_ARGS__;timeout_counter++, result = detect(DeviceIndex{at.device_index})) {\
        if (timeout_counter > 0xFFF0) return result;\
    }
    // IDE
    auto [chan, device] = diskID2chan_dev(DeviceIndex{at.device_index});
    u16 timeout_counter = 0;
    DetectTable result;

    _LM_act_timeout_guard(result.status.busy)

    if (at.fill_addr){
        if (at.if_need_CHS) {
            // well...
            outb(at.LBA.count, chan + CMDBlock::sector_num);
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

        _LM_act_timeout_guard(!result.status.ready || result.status.busy)
    }

    if (at.fill_device) {
        outb(device, chan + CMDBlock::device);
        _LM_act_timeout_guard(!result.status.ready || result.status.busy)
    }

    if (at.send_cmd) {
        outb(static_cast<u8>(at.cmd), chan + CMDBlock::CMD);
        _LM_act_timeout_guard(!result.status.ready || result.status.busy || result.status.error)
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

rtl::pair<DetectTable, size_t> IDE_control::pio_data_in(bufAddr buf, SectorCount count, DeviceIndex device) const {
    size_t bytes_read = 0;
    size_t timeout_count = 0;
    DetectTable result = detect(device);
    auto [chan, _] = diskID2chan_dev(device);

    for (auto sec : rtl::range{count}) {
        for (;result.status.busy || !result.status.data_ready;
            timeout_count++, result = detect(device)) {
            if (timeout_count > 512) return {result, 0};
        }
        for (size_t i = 0;i < 256;i++) {
            buf[i + static_cast<byteCount>(sec).to_size_t() / 2] = inw(chan + CMDBlock::data);
            bytes_read+=2;
        }
    }
    return {result, bytes_read};
}
rtl::pair<DetectTable, size_t> IDE_control::dma_data_in(bufAddr buf, SectorCount count, DeviceIndex device) const {
    ;
}
rtl::pair<DetectTable, size_t> IDE_control::pio_data_out(bufAddr buf, SectorCount count, DeviceIndex device) const {
    size_t bytes_write = 0;
    size_t timeout_count = 0;
    DetectTable result;
    auto [chan, _] = diskID2chan_dev(device);

    for (auto sec : rtl::range{count}) {
        for (;result.status.busy || !result.status.data_ready;
            timeout_count++, result = detect(device)) {
            if (timeout_count > 512) return {result, 0};
        }
        for (size_t i = 0;i < 256;i++) {
            outw(buf[i + static_cast<byteCount>(sec).to_size_t() / 2], chan + CMDBlock::data);
            bytes_write+=2;
        }
    }
    return {result, bytes_write};
}
rtl::pair<DetectTable, size_t> IDE_control::dma_data_out(bufAddr buf, SectorCount count, DeviceIndex device) const {
    ;
}

/*
rtl::pair<DetectTable, size_t> IDE_control::PIO_read_data(u16 *buf, u16 count, DetectTable result, DeviceIndex device_index) {
    auto [chan, _] = diskID2chan_dev(device_index);
    u8 timeout_count = 0;
    unsigned bytes_deal = 0;
    for(u16 fan = 0;fan < count;fan++){
        for (;result.status.busy || !result.status.data_ready;timeout_count++){
            result = detect(device_index);
            if(timeout_count > 128) return {result, bytes_deal};
        };
        for(u16 i = 0;i < 256;i++){
            buf[i + fan * 256] = inw(chan);
            bytes_deal += 2;
        }
    }
    return {detect(device_index), bytes_deal};
}
*/

IDE::IDE(const DiskInfo &diskinfo_, const IDE_control &ctl_, DeviceIndex device_):
diskinfo{diskinfo_}, ctl{ctl_}, device{device_} {}



unsigned IDE::read(void *buf, unsigned LBA, unsigned LBAh, unsigned count) {
#undef LocalDebug
#include <global/macro.hpp>
    LocalDebugInfo(cout << '[' << LBA << ", " << count << "]\n";)
    DetectTable detect{};
    size_t bytes = 0;
    if (diskinfo.address_mode & 0b10) {
        LocalDebugInfo(cout << "PIO mode: " << diskinfo.PIO_supported << '\n';)
        if (false) { // DMA no yet
            auto [d_, b_] = ctl.read_DMA_LBA(bufAddr{buf}, LBAaddr{LBAh, LBA}, SectorCount{(u16)count}, device);
            detect = d_; bytes = b_;
            goto DONE;
        } else if (diskinfo.PIO_supported || true) { // use PIO
            auto [d_, b_] = ctl.read_PIO_LBA(bufAddr{buf}, LBAaddr{LBAh, LBA}, SectorCount{(u16)count}, device);
            detect = d_; bytes = b_;
            goto DONE;
        }
    } else if (diskinfo.address_mode & 1) {
        auto sec_per_tr = diskinfo.CHS_sectors_per_track;
        auto tra_per_suf = diskinfo.CHS_cylinders;

        auto suf_id = LBA / (sec_per_tr * tra_per_suf);
        auto sec_off_on_suf = LBA % (sec_per_tr * tra_per_suf);
        auto tra_id = sec_off_on_suf / sec_per_tr;
        auto sec_id = sec_off_on_suf % sec_per_tr;

        for (u8 i = suf_id;i < diskinfo.CHS_heads;i++, tra_id = 0) {
            // for all surface
            for (u32 j = tra_id;j < tra_per_suf;j++, sec_id = 0) {
                // for all track
                size_t sec_read = min(sec_per_tr - sec_id, count);
                auto [d_, b_] = ctl.read_PIO_CHS(bufAddr{buf}, j, i, sec_id, SectorCount{(u16)sec_read}, device);
                detect = d_; bytes += b_;
                count -= sec_read;
                buf = (void *)((u8 *)buf + sec_read * 512);
                if (count == 0) goto DONE;
                if (detect.status.error) return -1;
            }
        }
    }
    LocalDebugInfo(cout << "can't read\n";)
    return -1;
DONE:
    if (detect.status.error || detect.status.busy) return -1;
    else return bytes;
}

unsigned IDE::write(void *buf, unsigned LBA, unsigned, unsigned count) {
    auto [detect, bytes] = ctl.write_PIO_LBA(bufAddr{buf}, LBAaddr{LBA, 0}, SectorCount{(u16)count}, device);
    if (detect.status.error) return -1;
    else return bytes;
}

DiskInfo *IDE::info(unsigned, String){
    return &diskinfo;
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