#include "_las.hpp"
#include <drv/disk/IDE/ide.hpp>

LAS *linear_address_space;

LAS::LAS():disks{1}, drivers{1}{
    // scan PCI bus to find disk and get driver object, 
    for(unsigned i = 0; i < PCI_device_numbers; i++){
        if(PCI_device_config_pointer[i].Class_code[2] == 0x01){
            if(PCI_device_config_pointer[i].Class_code[1] == 0x01){
                if(PCI_device_config_pointer[i].dev_drv)
                for(unsigned j = 0; j < PCI_device_config_pointer[i].size; j++){
                    disks.append(
                        {
                            &static_cast<IDE_DISK *>(PCI_device_config_pointer[i].dev_drv)[j], 
                            nullptr,
                            j
                        }
                    );
                }
                else screen->print("[WARNING] No driver\n");
            }
            else if (PCI_device_config_pointer[i].Class_code[1] == 0x02){
                // SATA disk not implemented yet
            }
        }
    }
    //partition
    char le[3] = {'A', ':', 0};
    for(auto [driver, partitions, id] : disks){
        partitions = new DISK_PART {driver};
        if(partitions->has_MBR) {
            //fs
            /*
            for(unsigned j = 0;j < 4;j++){
                auto fs = new FAT16{partitions, j};
                if (fs->status == FAT16::FORMAT){
                    drivers.append({fs, le, (id << 16) | j});
                    le[0]++;
                }
                else if (fs->status == FAT16::UNFORMAT || fs->status == FAT16::RAW){
                    DriveInfo di = {nullptr, {}, (id << 16) | j};
                    drivers.append(di);
                }
            }*/
        }
    }
    screen->print("[INFO] Found these disks:\n");
    show_driver();
    linear_address_space = this;
}

bool LAS::choose_disk(String &drive_letter){
    for(unsigned i = 0;i < drivers.get_size();i++){
        if(drivers[i].driver_letter == drive_letter){
            if(drivers[i].fs){
                dealing = drivers[i].fs;
                drive_letter+=static_cast<FileSystem *>(dealing)->dealing_path;
                is_fs = true;
                return true;
            }else{
                if(disks[i].driver){
                    dealing = disks[i].driver;
                    is_fs = false;
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
    /*
    cs->reg_cmd("dir", &__dir);
    cs->reg_cmd("mkfs.", &mkfs_);
    cs->reg_cmd("set", &alloc_driver_letter);
    */
}

void LAS::set_letter(unsigned disk_part_uid, String letter){
    if(((disk_part_uid >> 16) & 0xFFFF) >= disks.get_size()){
        screen->print("No this Disk!\n");
        return;
    }
    for(auto [fs, driver_letter, dpuid] : drivers){
        if(dpuid == disk_part_uid){
            driver_letter = letter;
            screen->print("Set Done: ");screen->print(driver_letter);
            print_char('\n');
            return;
        }
    }
    screen->print("No this part!\r\n");
}

unsigned LAS::set_part(unsigned disk_id, unsigned from_LBA, unsigned to_LBA, String le){
    DISK_ &disk = *disks[disk_id].driver;
    if(to_LBA == 0xFFFFFFFF)to_LBA = disk.info()->total_sectors;
    DISK_PART &part_manager = *disks[disk_id].partitions;
    u32 disk_part_uid = (disk_id << 16) | (part_manager.make_part(false, to_LBA, from_LBA) & 0xFFFF);
    DriveInfo di = {nullptr, {}, disk_part_uid};
    if(le != String{})di.driver_letter = le;
    drivers.append(di);
    return disk_part_uid;
}

void LAS::show_driver(){
    for(auto [_, driver_letter, disk_part_uid] : drivers){
        if(driver_letter.empty()){
            screen->print("ID(No letter): ");
            print_hex(disk_part_uid);
            print_char('\n');
        } else {
            screen->print(driver_letter);
            print_hex(disk_part_uid);
            print_char(' \n');
        }
    }
}



