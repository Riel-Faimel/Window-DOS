#pragma GCC optimize("Os")
#include "_ide.hpp"

void init_IDE_controller(DeviceConfig &) {
    new IDE_control{};
}

IDE_control::IDE_control() {
    // mm, ms, sm, ss
    for (u8 i = 0;i < 4;i++) {
        DebugInfo(cout << "Identify\n";)
        auto diskinfo = identify(i);
        DebugInfo(cout << "done\n";)
        if (diskinfo) {
            // disk exist
            auto disk = new IDE{*diskinfo, *this, i};
            partmanager->resolve(disk);
        } DebugInfo(else {
            cout << "null port\n";
        })
    }
}

DetectTable IDE_control::act(const ActionTable &at) {
    // IDE
    u16 chan = (at.device_index & 2u)?0x170:0x1F0;
    u8 device = (at.device_index & 1u)?0b10110000:0b10100000;

    LocalDebugInfo(cout << chan << ", " << device;)
    LocalDebugInfo(cout << "choose device\n";)
    outb(device, chan + CMDBlock::device);
    io_wait();
    auto result = detect(at.device_index & 2u);
    while(result.status.busy) {
        result = detect(at.device_index & 2u);
    }

    LocalDebugInfo(cout << "filled addr, sec\n";)
    if (at.fill_addr)
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

        if (at.LBA.high || lba48l_hf) {
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

    ATAstatus status = (ATAstatus)inb(chan + CMDBlock::status);
    while(!status.ready || status.busy) {
        status = (ATAstatus)inb(chan + CMDBlock::status);
    }
    LocalDebugInfo(cout << "send cmd\n";)
    outb(static_cast<u8>(at.cmd), chan + CMDBlock::CMD);
    return {
        .error = (ATAerror)inb(chan + CMDBlock::error),
        .status = (ATAstatus)inb(chan + CMDBlock::status),
    };
}

DetectTable IDE_control::detect(u8 device_index) {
    u16 chan = device_index?0x170:0x1F0;
    return {
        .error = (ATAerror)inb(chan + CMDBlock::error),
        .status = (ATAstatus)inb(chan + CMDBlock::status),
    };
}

u16 IDE_control::data_port_read(u8 device_index) {
    return inw(device_index?0x170:0x1F0 + CMDBlock::data);
}

void IDE_control::data_port_write(u16 val, u8 device_index) {
    outw(val, device_index?0x170:0x1F0 + CMDBlock::data);
}

IDE::IDE(DiskInfo &diskinfo_, IDE_control &ctl_, bool device_):
diskinfo{diskinfo_}, ctl{ctl_}, device{device_} {}

unsigned IDE::read(void *buf, unsigned LBA, unsigned, unsigned count) {
    DetectTable detect{};
    size_t bytes = 0;
    if (diskinfo.LBA_support) {
        if (diskinfo.PIO_supported) {
            auto [d_, b_] = ctl.read_PIO_LBA((u16 *)buf, LBA, 0, count, device);
            detect = d_; bytes = b_;
        } else {
            //auto [d_, b_] = ctl.read_DMA_LBA((u16 *)buf, LBA, 0, count, device);
            //detect = d_; bytes = b_;
        }
    } else {
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
    }
DONE:
    if (detect.status.error) return -1;
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



#pragma GCC optimize("Os")