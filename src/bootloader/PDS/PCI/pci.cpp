#include "_pci.hpp"

DeviceConfig &construc(PCI_device_config &pcf, PCI_space *enumer) {
    auto re = new DeviceConfig{ 
        .bus_type = DeviceConfig::BUS_TYPE::PCI,
        .class_type = DeviceConfig::CLASS_TYPE::nul, 
        .device_type = 0, .config = &pcf, .enumer = enumer 
    };
    switch (pcf.Class_code[2]){
    case 0x01: // stroage controller
        re->class_type = DeviceConfig::CLASS_TYPE::storage;
        switch (pcf.Class_code[1]) {
        case 0x01:
            re->device_type = _GD_storage_IDE_disk;
            break;
        default:
            break;
        }
        break;
    case 0x02: //network controller
        switch (pcf.Class_code[1]){
        case 0x00:
            break;
        default:
            break;
        }
        break;
    case 0x03:
        break;
    case 0x06:
        break;
    default:
        break;
    }
    return *re;
}

PCI_space::PCI_space(bool print_info): print_info(print_info){};

inline void PCI_space::probe(){
    for(int dev = 0;dev < 32;dev++){
        pci_probe_device(0, dev, print_info);
    }
    for (int bus = 1;bus < 256;bus++){
        if(pci_read(bus, 0, 0, 0) == 0xFFFF)continue;
        for (int dev = 0; dev < 32;dev++){
            pci_probe_device(bus, dev, print_info);
        }
    }
    if(!print_info){
        cout << "[INFO] PCI probe done: " << config.get_size() << " devices found\n";
    }
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

inline void PCI_space::pci_probe_device(int bus, int dev, bool print_info){
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

    if(print_info){
        cout << ("[INFO] VEN:DEV  REV  CLS\n");
        cout << cfg.vender_ID << ':' << cfg.Device_ID << ' ' << cfg.Revision_ID << ' ' 
        << static_cast<u32>(cfg.Revision_ID << 24 | cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]);
    }
    if(cfg.Header_type & 0x80){
        for(int func_id_ = 1;func_id_ < 8;func_id_++){
            unsigned int id = pci_read(bus, dev, func_id_, 0);
            if (
                id == 0xffffffff || id == 0x00000000 ||
                id == 0x0000ffff || id == 0xffff0000
            ) return;

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
            if(print_info){
                cout << "\n[MULTIFUNC] VEN:DEV  REV  CLS\n" << cfg.vender_ID << ':' << cfg.Device_ID << ' ' << cfg.Revision_ID << ' '
                << static_cast<u32>(cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]) << '\n';
            }
        }
    }
    if(print_info){
        cout << '\n';
    }
}

void PCI_space::set_driver(){
    for(auto &device : config) {
        auto &cf = construc(device, this);
        if (kmod->driver.exists(cf)) {
            kmod->driver[cf](cf);
        }
    };
}
