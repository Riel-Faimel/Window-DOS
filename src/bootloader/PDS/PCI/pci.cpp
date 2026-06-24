#include "_pci.hpp"

PCI_space::PCI_space(bool print_info){
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
        screen->print("[INFO] PCI device probe done: ");
        print_hex(static_cast<u16>(config.get_size()));
        screen->print(" devices found\r\n");
    }
};

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
    ) return ;

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
        screen->print("[INFO] VEN:DEV  REV  CLS\r\n");
        print_hex(cfg.vender_ID);
        screen->print(":");
        print_hex(cfg.Device_ID);
        screen->print("  ");
        print_hex(cfg.Revision_ID);
        screen->print("  ");
        print_hex(static_cast<u32>(cfg.Revision_ID << 24 | cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]));
    }
    if(cfg.Header_type & 0x80){
        for(int func_id_ = 1;func_id_ < 8;func_id_++){
            unsigned int id = pci_read(bus, dev, func_id_, 0);
            if (
                id == 0xffffffff || id == 0x00000000 ||
                id == 0x0000ffff || id == 0xffff0000
            ) return ;

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
                screen->print("\r\n[MULTIFUNC] VEN:DEV  REV  CLS\r\n");
                print_hex(cfg.vender_ID);
                screen->print(":");
                print_hex(cfg.Device_ID);
                screen->print("  ");
                print_hex(cfg.Revision_ID);
                screen->print("  ");
                print_hex(static_cast<u32>(cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]));
                screen->print("\r\n");
            }
        }
    }
    if(print_info){
        screen->print("\r\n");
    }
}

void PCI_space::set_device_driver(){
    for(auto device : config){
        switch (device.Class_code[2]){
        case 0x01: // stroage controller
            switch (device.Class_code[1]){
            case 0x00: //SCSI
                break;
            case 0x01: //IDE
                //screen->print("IDE controller\r\n");
                init_IDE_controller(&device.dev_drv, &device.size);
                break;
            case 0x02: //floppy
                screen->print("floppy controller\r\n");
                break;
            case 0x06: //STAT
                screen->print("SATA controller\r\n");
                break;
            case 0x08: //NVMe
                break;
            default:
                break;
            }
            break;
        case 0x02: //network controller
            switch (device.Class_code[1]){
            case 0x00:
                //screen->print("internet controller\r\n");
                break;
            default:
                break;
            }
            break;
        case 0x03:
            //screen->print("display controller\r\n");
            break;
        case 0x06:
        /*
            switch (device.Class_code[1]){
            case 0x00: //host bridge
                screen->print("Host bridge\n");
                break;
            case 0x01:
                screen->print("ISA bridge\n");
                break;
            case 0x02:
                screen->print("EISA bridge\n");
                break;
            case 0x03:
                screen->print("MicroChannal bridge\n");
                break;
            case 0x04:
                screen->print("PCI bridge\n");
                break;
            default:
                print_hex(static_cast<u16>(device.Class_code[1]<<8 | device.Class_code[0]));
                break;
            }
        */
            break;
        default:
            break;
        }
    };
}
