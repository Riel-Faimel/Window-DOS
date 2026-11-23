#pragma GCC optimize("Os")
#include "_pci.hpp"

InsideModuleRecord EnumerMod head {
    []() -> Enumerator *{ return new PCI_space{}; }
};

DeviceInfo *PCI_space::construc(PCI_device_config &pcf, PCI_space *enumer) {
    auto re = new DeviceInfo{
        .progif = ProgIFIndex{0},
        .bus = id,
        .cls = DeviceInfo::ClassType::nul, 
        .device = DeviceInfo::DeviceType{0}, 

        .config = &pcf,
        .enumer = enumer 
    };
    switch (pcf.class_code){
    case StorageController: {
        re->cls = DeviceInfo::ClassType::storage;
        switch (pcf.subclass_code) {
        case SCSIBus: {} break;
        case IDE: {
            // let PCI native support ISA
            //re->device = dev_hash(pcf.vender_ID, pcf.Device_ID, pcf.Revision_ID);
            re->progif = ProgIFIndex::PCI_IDE;
        } break;
        case FloppyDisk: {} break;
        case RAID: {} break;
        case ATA: {} break;
        case SATA: {
            //re->device = DeviceType{_GM_DeviceType_SATA};
            if (pcf.prog_interface == SATApi_AHCI || true) {
                re->progif = ProgIFIndex::PCI_AHCI;
            } else {
                re->progif = ProgIFIndex::PCI_IDE;
            }
        } break;
        default: {} break;
        } 
    } break;
    case NetworkController: {
        re->cls = DeviceInfo::ClassType::network;
        switch (pcf.subclass_code){
        case 0x00: {} break;
        default: {} break;
        }
    } break;
    case DisplayController: {} break;
    case MultiMediaController: {} break;
    case MemoryController: {} break;
    case Bridge: { ; } break;
    case SerialBusController: {} break;
    default: {} break;
    }
    return re;
}

PCI_space::PCI_space():progif_num{} {};

void PCI_space::probe(){
    for(int dev = 0;dev < 32;dev++){
        pci_probe_device(0, dev);
    }
    for (int bus = 1;bus < 256;bus++){
        if(pci_read(bus, 0, 0, 0) == 0xFFFF)continue;
        for (int dev = 0; dev < 32;dev++){
            pci_probe_device(bus, dev);
        }
    }
    
    kout << "[PCI] -probe " << (u16)config.get_size() << " Devices\n";
    // 用引用遍历：DeviceInfo::config 要指向列表里的元素，而不是循环临时量。
    LocalDebugInfo(
    unsigned storage_n = 0;
    size_t i = 0;
    )
    for (auto &dev : config) {
        LocalDebugInfo(
        if (dev.class_code == StorageController && i < addr_l.get_size()) {
            auto &a = addr_l[i];
            storage_n++;
            // 每个存储控制器一行：地址、class/subclass/progif、BAR5、命令寄存器。
            // 物理机 AHCI 找不到时，先看这一行有没有出现。
            kout << "[PCI] st ";
            kout.hex((u8)a.bus); kout << ':';
            kout.hex((u8)a.dev); kout << '.';
            kout.hex((u8)a.func);
            kout << ' ';
            kout.hex((u16)dev.vender_ID); kout << ':';
            kout.hex((u16)dev.Device_ID);
            kout << " cc=";
            kout.hex(dev.class_code);
            kout.hex(dev.subclass_code);
            kout.hex(dev.prog_interface);
            kout << " bar="; kout.hex(dev.BAR[5]);
            kout << " cmd="; kout.hex(dev.Command);
            kout << '\n';
        }
        ++i;
        )
        pdevspace->append(construc(dev, this));
    }
    LocalDebugInfo(if (!storage_n) kout << "[PCI] no storage class device\n";)
}

inline void PCI_space::pci_lookfor_addr(int bus, int dev, int func, int reg){
    unsigned int addr = (1<<31)|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC);
    outl(addr, PCI_address);
}

inline unsigned int PCI_space::pci_read(int bus, int dev, int func, int reg){
    pci_lookfor_addr(bus, dev, func, reg);
    io_wait();
    return inl(PCI_data);
}

inline void PCI_space::pci_write(int bus, int dev, int func, int reg, unsigned val){
    pci_lookfor_addr(bus, dev, func, reg);
    io_wait();
    outl(val, PCI_data);
    io_wait();
}

inline void PCI_space::pci_probe_device(int bus, int dev){
    unsigned int id = pci_read(bus, dev, 0, 0);
    if (
        id == 0xffffffff || id == 0x00000000 ||
	    id == 0x0000ffff || id == 0xffff0000
    ) return;

    unsigned time_count = 0;
    while (id == 0xffff0001) {
		io_wait();
        time_count++;
        id = pci_read(bus, dev, 0, 0);
        if (
            id == 0xffffffff || id == 0x00000000 ||
            id == 0x0000ffff || id == 0xffff0000
        ) return ;
		if (time_count > 0x80) return;
	}

    PCI_device_config cfg = {};
    reinterpret_cast<unsigned *>(&cfg)[0] = id;
    for(int i = 1;i < 16;i++){
        reinterpret_cast<unsigned *>(&cfg)[i] = pci_read(bus, dev, 0, i*4);
    }
    config.append(cfg);
    addr_l.append(PCI_addr{(u8)bus, (u8)dev, (u8)0});

    LocalDebugInfo(
        kout << ("[PCI] VEN:DEV  REV  CLS\n");
        kout << cfg.vender_ID << ':' << cfg.Device_ID << ' ' << cfg.Revision_ID << ' ' 
        << static_cast<u32>(cfg.Revision_ID << 24 | cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]) << '\n';
    )
    if(cfg.Header_type & 0x80){
        for(int func_id_ = 1;func_id_ < 8;func_id_++){
            unsigned int id = pci_read(bus, dev, func_id_, 0);
            if (
                id == 0xffffffff || id == 0x00000000 ||
                id == 0x0000ffff || id == 0xffff0000
            ) {
                // 多 function 设备里空 function 是正常的，不能在这里收摊。
                // 物理机 PCH 常见 .0 LPC / .2 SATA / .3 SMBus（.1 空），
                // 这行出现就说明后面的 function 被丢掉了。
                kout << "[PCI] mf ";
                kout.hex((u8)bus); kout << ':';
                kout.hex((u8)dev); kout << '.';
                kout.hex((u8)func_id_);
                kout << " gap, rest skipped\n";
                return;
            }

            unsigned time_count = 0;
            while (id == 0xffff0001) {
                io_wait();
                time_count++;
                id = pci_read(bus, dev, func_id_, 0);
                if (
                    id == 0xffffffff || id == 0x00000000 ||
                    id == 0x0000ffff || id == 0xffff0000
                ) return ;
                if (time_count > 0x80) return;
            }

            PCI_device_config cfg = {};
            reinterpret_cast<unsigned *>(&cfg)[0] = id;
            for(int i = 1;i < 16;i++){
                reinterpret_cast<unsigned *>(&cfg)[i] = pci_read(bus, dev, func_id_, i*4);
            }
            config.append(cfg);
            addr_l.append(PCI_addr{(u8)bus, (u8)dev, (u8)func_id_});
            LocalDebugInfo(
                kout << "[MULTIFUNC] VEN:DEV  REV  CLS\n" << cfg.vender_ID << ':' << cfg.Device_ID << ' ' << cfg.Revision_ID << ' '
                << static_cast<u32>(cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]) << '\n';
            )
        }
    }
}

PCI_space::PCI_addr *PCI_space::find_addr(void *cfg) {
    size_t i = 0;
    for (auto &c : config) {
        if (static_cast<void *>(&c) == cfg)
            return i < addr_l.get_size() ? &addr_l[i] : nullptr;
        ++i;
    }
    return nullptr;
}

void PCI_space::set_mod(void *cfg) {
    auto addr = find_addr(cfg);
    if (!addr) {
        kout << "[PCI] set_mod: unknown device\n";
        return;
    }

    constexpr int command_reg = 0x04;
    constexpr unsigned need =
        static_cast<unsigned>(Commmand_bits::command_memory_space) |
        static_cast<unsigned>(Commmand_bits::command_bus_master);

    unsigned cmd = pci_read(addr->bus, addr->dev, addr->func, command_reg);
    if ((cmd & need) != need) {
        pci_write(addr->bus, addr->dev, addr->func, command_reg, cmd | need);
        cmd = pci_read(addr->bus, addr->dev, addr->func, command_reg);
    }

    // Intel ICH7 SATA：PCS(0x92) 的 bit0-3 是端口时钟使能。
    // BIOS 可能只给启动端口开时钟，甚至全关；端口时钟没开时
    // PxSSTS 永远读 0，COMRESET 也不会建立链路。
    // PCI 配置里的 0x90 是 dword：MAP(0x90)、PCS(0x92)、IR(0x94)。
    auto *pc = static_cast<PCI_device_config *>(cfg);
    if (pc && pc->vender_ID == 0x8086 && (pc->Device_ID & 0xFFF8u) == 0x27C0u) {
        unsigned d = pci_read(addr->bus, addr->dev, addr->func, 0x90);
        unsigned pcs = (d >> 16) & 0xFFu;
        if ((pcs & 0x0Fu) != 0x0Fu) {
            pci_write(addr->bus, addr->dev, addr->func, 0x90, d | (0x0Fu << 16));
            d = pci_read(addr->bus, addr->dev, addr->func, 0x90);
        }
        kout << "[PCI] PCS "; kout.hex((u8)pcs);
        kout << "->"; kout.hex((u8)((d >> 16) & 0xFFu)); kout << '\n';
    }

    LocalDebugInfo(
        kout << "[PCI] mod " << (u32)addr->bus << ':' << (u32)addr->dev
             << '.' << (u32)addr->func << " cmd=";
        kout.hex((u16)cmd);
        kout << '\n';
    )
}
