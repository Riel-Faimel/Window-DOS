#include "_pci.hpp"

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
    
#define LocalDebug
#include <global/macro.hpp>
    LocalDebugInfo(
        kout << "[PCI] -probe " << (u16)config.get_size() << " Devices\n";
    )
#undef LocalDebug
#include <global/macro.hpp>
    for (auto dev : config) {
        pdevspace->append(construc(dev, this));
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
            LocalDebugInfo(
                kout << "[MULTIFUNC] VEN:DEV  REV  CLS\n" << cfg.vender_ID << ':' << cfg.Device_ID << ' ' << cfg.Revision_ID << ' '
                << static_cast<u32>(cfg.Class_code[0]<<16 | cfg.Class_code[1] << 8 | cfg.Class_code[2]) << '\n';
            )
        }
    }
}
