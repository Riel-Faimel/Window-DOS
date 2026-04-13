#include "_las.hpp"

LAS *linear_address_space;

LAS::LAS():disks(nullptr), disk_num(0){
    rtl::array<DiskInfo> disk_array(1);
    // scan PCI bus to find disk and get driver object, 
    for(unsigned i = 0; i < PCI_device_numbers; i++){
        if(PCI_device_config_pointer[i].Class_code[2] == 0x01){
            if(PCI_device_config_pointer[i].Class_code[1] == 0x01){
                for(unsigned j = 0; j < PCI_device_config_pointer[i].size; j++){
                    disk_array.append({&(static_cast<IDE_DISK *>(PCI_device_config_pointer[i].dev_drv)[j]), nullptr, nullptr, 0, {}});
                    disk_num++;
                }
            }
            else if (PCI_device_config_pointer[i].Class_code[1] == 0x02){
                // SATA disk not implemented yet
            }
        }
    }
    //partition
    disks = disk_array.get_ptr();
    disk_num = disk_array.get_size();
    for(unsigned i = 0;i < disk_num;i++){
        disks[i].partitions = new DISK_PART {static_cast<DISK_ *>(disks[i].driver)};
        if(static_cast<DISK_PART *>(disks[i].partitions)->has_MBR) {
            //fs
            for(unsigned j = 0;j < 4;j++){
                auto fs = new FAT16{static_cast<DISK_PART *>(disks[i].partitions), j};
                if (fs->status == FAT16::FORMAT){
                    disks[i].fs = fs;
                    char le[3] = {'A', ':', 0};
                    le[0]++;
                    set_letter(i, {le});
                }
            }
        }
    }
    screen->print("\r\n[INFO] Found these disks:\n");
    for(unsigned i = 0;i < disk_num;i++){
        if(disks[i].drvier_letter.empty()){
            screen->print("ID: ");
            print_hex(i);
            print_char('\n');
        } else {
            screen->print(disks[i].drvier_letter);
            print_char('\n');
        }
    }
    linear_address_space = this;
}

void LAS::mkfs_FAT16(unsigned char disk_id, unsigned part_id){
    DiskInfo &disk = disks[disk_id];
    disk.fs = new FAT16{static_cast<DISK_PART *>(disk.partitions), part_id, true, true};
    disk.fs_num++;
    screen->print("Disk ");screen->print(disk.drvier_letter);screen->print(" format into FAT16\n");
}

bool LAS::choose_disk(String &drive_letter){
    for(unsigned i = 0;i < disk_num;i++){
        if(disks[i].drvier_letter == drive_letter){
            if(disks[i].fs){
                dealing = disks[i].fs;
                drive_letter+="\\";
                return true;
            }else{
                if(disks[i].driver){
                    dealing = disks[i].driver;
                    drive_letter+="|";
                    screen->print("[WARNING] NO File System\n");
                    return true;
                }else{
                    screen->print("[ERROR] NO driver\n");
                    return false;
                }
            }
        }
    }
    screen->print("[FAILED] NO driver letter\n");
    return false;
}

unsigned int LAS::open(String filename){
    ;
}

void LAS::reg_cmd(CenterShell *cs){
    cs->reg_cmd("dir", &__dir);
    cs->reg_cmd("mkfs.", &mkfs_);
    cs->reg_cmd("set", &alloc_driver_letter);
}

void LAS::set_letter(unsigned int disk_id, String letter){
    if(disk_id > disk_num){
        screen->print("NULL Disk!\n");
        return;
    }
    disks[disk_id].drvier_letter = letter;
    screen->print("Set Done: ");screen->print(disks[disk_id].drvier_letter);
    print_char('\n');
}



