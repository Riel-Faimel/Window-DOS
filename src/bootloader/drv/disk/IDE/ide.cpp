#pragma GCC optimize("O2")
#include "_ide.hpp"

void init_IDE_controller(DeviceConfig &) {
    new IDE_control{};
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
    cout << "Disk, ";
    if (diskinfo->PIO_supported) {
        cout << "PIO mode\n";
    } else {
        cout << "Unknown Mode\n";
    }
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
            partmanager->resolve(disk);
            ASMLabelMark(resolve_done)
#undef LocalDebug
#include <global/macro.hpp>
        } LocalDebugInfo(else {
            cout << "null port\n";
        })
    }
}

__attribute__((optimize("O0")))
DetectTable IDE_control::act(const ActionTable &at) {
    // IDE
    auto [chan, device] = diskID2chan_dev(at.device_index);
    u8 timeout_counter = 0;
    DetectTable result;

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
            lba28e = lba48l & 0x0Fu, lba48l_hf = (lba48l >> 4) & 0x0Fu,
            lba48m = static_cast<u8>(at.LBA.high), lba48h = static_cast<u8>(at.LBA.high >> 8);

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

        result = detect(at.device_index);
        for (;!result.status.ready || result.status.busy;timeout_counter++) {
            if (timeout_counter > 128u) return result;
            result = detect(at.device_index);
        }
    }

    if (at.fill_device) {
        outb(device, chan + CMDBlock::device);
        result = detect(at.device_index);
        for (;!result.status.ready || result.status.busy;timeout_counter++) {
            if (timeout_counter > 128u) return result;
            result = detect(at.device_index);
        }
    }

    if (at.send_cmd) {
        outb(static_cast<u8>(at.cmd), chan + CMDBlock::CMD);
        for (;!result.status.ready || result.status.busy || result.status.error;timeout_counter++) {
            if (timeout_counter > 128u || result.status.error) return result;
            result = detect(at.device_index);
        }
    }

    return result;
}

__attribute__((optimize("O0")))
DetectTable IDE_control::detect(u8 device_index) {
    auto [chan, _] = diskID2chan_dev(device_index);
    return {
        .error = (ATAerror)inb(chan + CMDBlock::error),
        .status = (ATAstatus)inb(chan + CMDBlock::status),
    };
}

u16 IDE_control::data_port_read(u8 device_index) {
    auto [chan, _] = diskID2chan_dev(device_index);
    return inw(chan + CMDBlock::data);
}

void IDE_control::data_port_write(u16 val, u8 device_index) {
    auto [chan, _] = diskID2chan_dev(device_index);
    outw(val, chan + CMDBlock::data);
}

IDE::IDE(DiskInfo &diskinfo_, IDE_control &ctl_, u8 device_):
diskinfo{diskinfo_}, ctl{ctl_}, device{device_} {}

unsigned IDE::read(void *buf, unsigned LBA, unsigned, unsigned count) {
    DetectTable detect{};
    size_t bytes = 0;
    if (diskinfo.address_mode & 0b10) {
        LocalDebugInfo(cout << "PIO mode: " << diskinfo.PIO_supported << '\n';)
        if (diskinfo.PIO_supported || true) {
            auto [d_, b_] = ctl.read_PIO_LBA((u16 *)buf, LBA, 0, count, device);
            detect = d_; bytes = b_;
        } /* else {
            auto [d_, b_] = ctl.read_DMA_LBA((u16 *)buf, LBA, 0, count, device);
            detect = d_; bytes = b_;
        } */
    } else if (diskinfo.address_mode & 1) {
        auto sec_per_tr = diskinfo.CHS_sectors_per_track;
        auto tra_per_suf = diskinfo.CHS_cylinders;

        //if (sec_per_tr==0 || tra_per_suf==0) throw 0;

        auto suf_id = LBA / (sec_per_tr * tra_per_suf);
        auto sec_off_on_suf = LBA % (sec_per_tr * tra_per_suf);
        auto tra_id = sec_off_on_suf / sec_per_tr;
        auto sec_id = sec_off_on_suf % sec_per_tr;

        for (u8 i = suf_id;i < diskinfo.CHS_heads;i++, tra_id = 0) {
            // for all surface
            for (u32 j = tra_id;j < tra_per_suf;j++, sec_id = 0) {
                // for all track
                size_t sec_read = min(sec_per_tr - sec_id, count);
                auto [d_, b_] = ctl.read_PIO_CHS((u16 *)buf, j, i, sec_id, sec_read, device);
                detect = d_; bytes += b_;
                count -= sec_read;
                buf = (void *)((u8 *)buf + sec_read * 512);
                if (count == 0) goto DONE;
                if (detect.status.error) return -1;
            }
        }
    } else {
        LocalDebugInfo(cout << "can't read\n";)
        return -1;
    }
DONE:
    if (detect.status.error || detect.status.busy) return -1;
    else return bytes;
}

unsigned IDE::write(void *buf, unsigned LBA, unsigned, unsigned count) {
    auto [detect, bytes] = ctl.write_PIO_LBA((u16 *)buf, LBA, 0, count, device);
    if (detect.status.error) return -1;
    else return bytes;
}

DiskInfo *IDE::info(unsigned, String){
    return &diskinfo;
}

unsigned IDE::cmd(unsigned, String, void *, unsigned) {
    return 0;
}

rtl::pair<u16, u8> IDE_control::diskID2chan_dev(u8 device_index) {
    // mm, ms, sm, ss
    return {
        (device_index & 2u)?(u16)0x170:(u16)0x1F0,
        (device_index & 1u)?(u8)0b10110000:(u8)0b10100000
    };
}


#pragma GCC optimize("Os")