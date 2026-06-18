#include "_ide.hpp"
#include <IDT_L/IDNT.hpp>
inline bool is_power_of_2(u32 n){
    return (n != 0) && ((n & (n - 1)) == 0);
}
extern "C" void when_PATA_Master_cut_handler();
extern "C" void when_PATA_Slave_cut_handler();
constexpr u16 ctl = 0x206;

IDE_Channal::IDE_Channal(IDE_DISK& master, IDE_DISK& slave, Channal chan_, IDT& idt):
chan(chan_){
    switch (chan_) {
    case Channal::Master_Channel:
        idt.regist(&when_PATA_Master_cut_handler, static_cast<unsigned >(IDNT::ATA_Master));
        break;
    case Channal::Slave_Channel:
        idt.regist(&when_PATA_Slave_cut_handler, static_cast<unsigned >(IDNT::ATA_Slave));
        break;
    default:
        return;
    }

    IDE_DISK&& mm = IDE_DISK{IDE_DISK::Device::Master_Device, idt, this};
    if(mm.exist)master = rtl::move(mm);
    else if(registry.do_IDE_controller_initialization_print_info)screen->print("[NOTICE] Master disk not found\r\n");
    IDE_DISK&& ms = IDE_DISK{IDE_DISK::Device::Slave_Device, idt, this};
    if(ms.exist)slave = rtl::move(ms);
    else if(registry.do_IDE_controller_initialization_print_info)screen->print("[NOTICE] Slave disk not found\r\n");
}

/**
 * from Linux 2.6.32.1
 * driver/ata/libata-sff.c
 */
/**
 *	ata_sff_tf_read - input device's ATA taskfile shadow registers
 *	@ap: Port from which input is read
 *	@tf: ATA taskfile register set for storing input
 *
 *	Reads ATA taskfile registers for currently-selected device
 *	into @tf. Assumes the device has a fully SFF compliant task file
 *	layout and behaviour. If you device does not (eg has a different
 *	status method) then you will need to provide a replacement tf_read
 *
 *	LOCKING:
 *	Inherited from caller.
 */
void IDE_Channal::ata_sff_tf_read(IDE_Channal::ata_taskfile &tf){
	tf.command = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS));
	tf.feature = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_ERR));
	tf.nsect = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_NSECT));
	tf.lbal = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAL));
	tf.lbam = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAM));
	tf.lbah = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAH));
	tf.device = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_DEVICE));

	if (tf.flags & static_cast<u8>(VALUE::ATA_TFLAG_LBA48)) {
		outb(tf.ctl | static_cast<u8>(VALUE::ATA_HOB), static_cast<u16>(chan) + static_cast<u8>(VALUE::ATA_PCI_CTL_OFS));
		tf.hob_feature = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_ERR));
		tf.hob_nsect = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_NSECT));
		tf.hob_lbal = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAL));
		tf.hob_lbam = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAM));
		tf.hob_lbah = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAH));
		outb(tf.ctl, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_PCI_CTL_OFS));
	}
}
/**
 *	ata_sff_tf_load - send taskfile registers to host controller
 *	@ap: Port to which output is sent
 *	@tf: ATA taskfile register set
 *
 *	Outputs ATA taskfile to standard ATA host controller.
 *
 *	LOCKING:
 *	Inherited from caller.
 */
void IDE_Channal::ata_sff_tf_load(IDE_Channal::ata_taskfile &tf){
	unsigned int is_addr = tf.flags & static_cast<u8>(VALUE::ATA_TFLAG_ISADDR);

	if (is_addr && (tf.flags & static_cast<u8>(VALUE::ATA_TFLAG_LBA48))) {
		outb(tf.hob_feature, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_FEATURE));
		outb(tf.hob_nsect, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_NSECT));
		outb(tf.hob_lbal, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAL));
		outb(tf.hob_lbam, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAM));
		outb(tf.hob_lbah, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAH));
	}

	if (is_addr) {
		outb(tf.feature, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_FEATURE));
		outb(tf.nsect, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_NSECT));
		outb(tf.lbal, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAL));
		outb(tf.lbam, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAM));
		outb(tf.lbah, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAH));
	}

	if (tf.flags & static_cast<u8>(VALUE::ATA_TFLAG_DEVICE)) {
		outb(tf.device, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_DEVICE));
	}
    while(inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS)) & static_cast<u8>(VALUE::ATA_BUSY));
}
/**
 *	ata_sff_exec_command - issue ATA command to host controller
 *	@ap: port to which command is being issued
 *	@tf: ATA taskfile register set
 *
 *	Issues ATA command, with proper synchronization with interrupt
 *	handler / other threads.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */
void IDE_Channal::ata_sff_exec_command(IDE_Channal::ata_taskfile &tf){
	outb(tf.command, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_CMD));
    inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS));
}

void IDE_Channal::read(unsigned short *buf, unsigned int LBA, unsigned char count, DISK_INFO *info){
    if(info->LBA_support)read_PIO_LBA(buf, LBA, count, info->device);
    else read_PIO_CHS(buf, LBA, count, info->device);
}

inline void IDE_Channal::read_PIO_LBA(unsigned short *buf, unsigned int LBA, unsigned char count, u8 dev){
    unsigned char status = inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS));
    if(status & 0x80){
        unsigned i = 0;
        while ((inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS)) & 0x80)){
            i++;
            if(i > 0x10000){
                if(registry.do_IDE_controller_initialization_print_info)screen->print("[NOTICE] time out\n");
                return; //time out
            }
        };    
    }
    ata_taskfile tf{
        .flags = static_cast<u32>(VALUE::ATA_TFLAG_ISADDR) | 
        static_cast<u32>(VALUE::ATA_TFLAG_DEVICE) | 
        static_cast<u32>(VALUE::ATA_TFLAG_LBA),
        .protocol = static_cast<u8>(VALUE::ATA_PROT_FLAG_PIO),
        .ctl = 0,
        .nsect = count,
        .lbal = (u8)(LBA & 0xFF),
        .lbam = (u8)((LBA >> 8) & 0xFF),
        .lbah = (u8)((LBA >> 16) & 0xFF),
        .device = dev | (u8)((LBA >> 24) & 0x0F),
        .command = static_cast<u8>(VALUE::ATA_CMD_PIO_READ)
    };
    ata_sff_tf_load(tf);
    ata_sff_exec_command(tf);

    for(unsigned char fan = 0;fan < count;fan++){
        unsigned i = 0;
        while (!(inb(static_cast<u16>(chan) + static_cast<u8>(VALUE::ATA_REG_STATUS)) & 0x08)){
            i++;
            if(i > 10000){
                if(registry.do_IDE_controller_initialization_print_info)kprint("[NOTICE] Time out\n");
                return;
            }
        };
        for(unsigned int i = 0;i < 256;i++){
            buf[i + fan * 256] = inw(static_cast<u16>(chan));
        }
    }
}

inline void IDE_Channal::read_PIO_CHS(unsigned short */*buf*/, unsigned int /*LBA*/, unsigned char /*count*/, u8 /*dev*/){
    kprint("[NOTICE] CHS read not implemented yet\r\n");
}

void IDE_Channal::write(unsigned short *buf, unsigned int LBA, unsigned char count, DISK_INFO *info){
    if(info->LBA_support)write_PIO_LBA(buf, LBA, count, info->device);
    else write_PIO_CHS(buf, LBA, count, info->device);
}

inline void IDE_Channal::write_PIO_LBA(unsigned short *buf, unsigned int LBA, unsigned char count, u8 dev){
    unsigned char status = inb(static_cast<u16>(chan) + static_cast<u8>(VALUE::ATA_REG_STATUS));
    if(status & 0x80){
        while ((inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS)) & 0x80));        
    }
    outb(count, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_NSECT));
    outb(LBA & 0xFF, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAL));
    outb((LBA >> 8) & 0xFF, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAM));
    outb((LBA >> 16) & 0xFF, static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_LBAH));
    outb(dev | ((LBA >> 24) & 0x0F), static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_DEVICE));
    outb(static_cast<char>(VALUE::ATA_CMD_PIO_WRITE), static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_CMD));

    for(unsigned char fan = 0;fan < count;fan++){
        unsigned i = 0;
        while (!(inb(static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_STATUS)) & 0x08)){
            i++;
            if(i > 10000){
                if(registry.do_IDE_controller_initialization_print_info)kprint("[NOTICE] Time out\r\n");
                break;
            }
        };
        for(unsigned int i = 0;i < 256;i++){
            outw(buf[i + fan * 256], static_cast<u16>(chan) + static_cast<u16>(VALUE::ATA_REG_DATA));
        }
    }
}

inline void IDE_Channal::write_PIO_CHS(unsigned short */*buf*/, unsigned int /*LBA*/, unsigned char /*count*/, u8 /*dev*/){
    kprint("[NOTICE] CHS write not implemented yet\r\n");
}

//=====================================================================

IDE_DISK &IDE_DISK::operator=(IDE_DISK &&other){
    if(this == &other)return *this;
    lock = other.lock;
    info_ = other.info_;
    exist = other.exist;

    other.lock = nullptr;
    other.info_ = {};
    other.exist = false;
    return *this;
}

IDE_DISK::IDE_DISK():exist{false}, lock(nullptr), info_() {}

IDE_DISK::IDE_DISK(Device dev, IDT &/*idt*/, IDE_Channal *c):
exist{true}, lock(c), info_{}{
    outb(static_cast<u8>(dev), static_cast<u16>(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_DEVICE)));
    io_wait();
    auto status = inb(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_STATUS));
    if(status == 0xFF){
        if(registry.do_IDE_controller_initialization_print_info)screen->print("[ERROR] No disk found\n");
        exist = false;
        return;
    }
#pragma pack(push, 1)
    union {
        u16 id[256];
        struct {
            u16 general_config;
            u16 CHS_cylinders;
            u16 _re;
            u16 CHS_heads;
            u16 __re[2];
            u16 CHS_sectors_per_track;
            u16 re_[3];
            u16 serial_number[10];
            u16 re__[3];
            u16 firmware_rev[4];
            u16 model[20];
            u16 rw_multiple;
            u16 re___;
            u16 support;
            u16 capabilities2;
            u16 pio_timing;
            u16 dma_timing;
            u16 fields_valid;
            u16 current_cylinders;
            u16 current_heads;
            u16 current_sectors;
            u32 current_chs_sectors;
            u16 rw_multiple_setting;
            u32 LBA28_sectors;
            u16 sw_dma_modes;
            u16 mw_dma_modes;
            u16 PIO_supported;
            u16 min_mw_dma_cycle;
            u16 min_mw_dma_cycle2;
            u16 min_pio_cycle;
            u16 min_pio_cycle_iordy;
            u16 re____[6];
            u16 queue_depth;
            u16 sata_cap[4];
            u16 major_version;
            u16 minor_version;
            u16 cmd_set1;
            u16 cmd_set2;
            u16 cmd_set3;
            u16 cmd_set4;
            u16 cmd_set5;
            u16 cmd_set6;
            u16 UDMA_modes;
            u16 secure_erase_time[4];
            u16 re_____[7];
            u32 LBA48_sectors_low;
            u32 LBA48_sectors_high;
            u16 stream_min_req;
            u16 stream_transfer_time;
            u16 logical_sector_size;
            u16 inter_seek_delay;
            u32 wwn[4];
            u16 re______[5];
            u32 words_per_sector;
            u16 cmd_set7;
            u16 cmd_set8;
            u16 re_______[6];
            u16 removable_status;
            u16 security_status;
            u16 vendor_specific[31];
            u16 cfa_power_mode;
            u16 cfa_key_mgmt;
            u16 cfa_modes;
            u16 cfa_trans;
            u16 cfa_mdata1;
            u16 cfa_mdata2;
            u16 cfa_mdata3;
            u16 cfa_mdata4;
            u16 cfa_reserved;
            u16 re__8[7];
            u16 media_serial[30];
            u16 sct_cmd_support[4];
            u16 re__9[7];
            u16 nominal_rotation_rate;
            u16 re__10[4];
            u16 transport_major;
            u16 transport_minor;
            u16 re__11[10];
            u16 min_prefetch;
            u16 max_prefetch;
            u16 re__12[19];
            u16 integrity_word;
        } identify_info;
    };
#pragma pack(pop)

//read_identify:
    outb(static_cast<u8>(dev), static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_DEVICE));
    unsigned i = 0;
    while(inb(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_STATUS)) & 0x80){
        i++;
        if(i > 0x10000){
            if(registry.do_IDE_controller_initialization_print_info)screen->print("[NOTICE] time out\n");
            exist = false;
            return; //time out
        }
    }; 
    // wait until device is not busy
    outb(static_cast<u8>(0xEC), static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_CMD)); 
    // send identify command

    i = 0;
    while(!(inb(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_STATUS)) & 0x08)){
        i++;
        if(i > 0x10000){
            if(registry.do_IDE_controller_initialization_print_info)screen->print("[NOTICE] time out\n");
            exist = false;
            return; //time out
        }
    }; 
    // 在读取数据之前应该检查 ERR 位
    status = inb(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_STATUS));
    if(status & 0x01) {
        if(registry.do_IDE_controller_initialization_print_info)screen->print("[ERROR] IDENTIFY command failed\n");
        exist = false;
        return;
    }
    for(int i = 0;i < 256;i++){
        id[i] = inw(static_cast<u16>(c->chan) + static_cast<u8>(VALUE::ATA_REG_DATA));
    }

//full_info_:
    info_.LBA_support = (identify_info.support >> 9) & 0x1;
    info_.total_sectors = identify_info.LBA28_sectors;
    info_.PIO_supported = identify_info.PIO_supported & 0x8000 ? identify_info.PIO_supported & 0b11111111  : 0;
    for(int i = 0;i < 40;i++){
        info_.model[i] = reinterpret_cast<char *>(identify_info.model)[i];
    }

    /**
     * from Linux2.6.32.1
     * driver/ata/libata-core.c
     */
    /* ATA-specific feature tests */
	if (
        identify_info.general_config == 0x848A ||
        identify_info.general_config == 0x844A ||
        identify_info.general_config == 0x045A
    ) { //兼容CF卡的小玩意
		/* CPRM may make this media unusable */
		if (id[static_cast<u32>(VALUE::ATA_ID_CFA_KEY_MGMT)] & 1)
        if(registry.do_IDE_controller_initialization_print_info)screen->print("supports DRM functions and may not be fully accessable.\n");
	} else {
		/* Warn the user if the device has TPM extensions */
		if (identify_info.cmd_set2 & 0b0001)
        if(registry.do_IDE_controller_initialization_print_info)screen->print("supports DRM functions and may not be fully accessable.\n");
	}

	info_.total_sectors = identify_info.LBA28_sectors;

    u8 LBA_mode = 0b01000000;
	/* get current R/W Multiple count setting */
	if ((id[47] >> 8) == 0x80 && (id[59] & 0x100)) {
		unsigned int max = id[47] & 0xFF;
		unsigned int cnt = id[59] & 0xFF;
		/* only recognize/allow powers of two here */
		if (is_power_of_2(max) && is_power_of_2(cnt)) 
        if (cnt <= max)
		info_.multi_count = cnt;
	}

	if (identify_info.support & (1 << 9)) {
        info_.LBA_support = 1;
		if (identify_info.cmd_set2 & (1 << 10)) {
			info_.LBA_support = 2;
            if(registry.do_IDE_controller_initialization_print_info)screen->print("supports LBA48\n");
		}else 
        if(registry.do_IDE_controller_initialization_print_info)screen->print("supports LBA\n");
	} else {
		/* CHS */
        if(registry.do_IDE_controller_initialization_print_info)screen->print("does not support LBA, using CHS\n");
        LBA_mode = 0;

		/* Default translation */
		info_.CHS_cylinders	= identify_info.current_cylinders;
		info_.CHS_heads	= identify_info.current_heads;
		info_.CHS_sectors_per_track	= identify_info.current_sectors;

		if (identify_info.fields_valid & 0b01) {
			/* Current CHS translation is valid. */
			info_.CHS_cylinders = identify_info.CHS_cylinders;
			info_.CHS_heads = identify_info.CHS_heads;
			info_.CHS_sectors_per_track = identify_info.CHS_sectors_per_track;
		}
	}
    info_.device = static_cast<u8>(dev);
};

unsigned IDE_DISK::read(void *buf, unsigned int LBA, unsigned /*byte_offset*/, unsigned /*byte_read*/){
    unsigned count = 0;
    if(lock)lock->read(static_cast<unsigned short *>(buf), LBA, count, &info_);
}

unsigned IDE_DISK::write(void *buf, unsigned int LBA, unsigned /*byte_offset*/, unsigned /*byte_read*/){
    unsigned count = 0;
    if(lock)lock->write(static_cast<unsigned short *>(buf), LBA, count, &info_);
}

DISK_INFO* IDE_DISK::info(String){
    return &info_;
}

unsigned IDE_DISK::cmd(unsigned int, String) {
    return 0;
}

void IDE_DISK::check(){
    outb(info_.device, static_cast<u16>(lock->chan) + static_cast<u16>(VALUE::ATA_REG_DEVICE));
    io_wait();
    io_wait();
    
    auto status = inb(static_cast<u16>(lock->chan) + static_cast<u16>(VALUE::ATA_REG_STATUS));
    
    if(status == 0xFF) {
        screen->print("-- No Device --\n");
        return;
    }
    
    if(status & 0x80) screen->print("-- Device Busy --\n");
    if(status & 0x40) screen->print("-- Device Ready --\n");
    if(status & 0x20) screen->print("-- Device Fault --\n");
    if(status & 0x10) screen->print("-- Data Request Ready --\n");
    if(status & 0x08) screen->print("-- Device Error --\n");
    
    // 空闲状态：不忙、无错误、无数据请求
    if(!(status & 0x80) && !(status & 0x08) && !(status & 0x10)) {
        screen->print("-- Device idle --\n");
    }
    
    // 打印完整状态值（调试用）
    print_hex(status);
}
