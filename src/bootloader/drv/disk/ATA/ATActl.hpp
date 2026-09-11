#include "ATAcmd.hpp"
#include <interface/dev_cls/cluster.hpp>
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <drv/screen/screen_srv.hpp>

struct ActionTable {
    struct {
        void *addr = 0;
        size_t size = 0;
    } buffer = {};
    
    // 64 bits till here

	struct {
        u32 low = 0;
        u16 high = 0;
        u16 count = 0;
	} LBA = {};

    // 64 bits till here

    u16 feature = 0;
    struct {
        u16 cylinder = 0;
        u8 sector = 0;
    } CHS = {};
    ATAcmd cmd;
    /**
     * for IDE, it is bool which point to master disk if false
     * or slave disk if true.
     * for AHCI, it is index of mmap of CMD block, also called 
     * SATA port index
     * 
     * so in range 0~1 or 0~32
     */
    u8 device_index : 4 = 0;
    u8 head : 4 = 0;
    u8 if_need_CHS : 1 = false;
    u8 fill_addr : 1 = true;
    u8 re : 6 = 0;
    // 64 bits till here
};

struct DetectTable {
    ATAerror error;
    ATAstatus status;
};

class ATADisk {
    virtual DetectTable act(const ActionTable &) = 0;
    virtual DetectTable detect(u8 device_index) = 0;
    virtual inline u16 data_port_read(u8) = 0;
    virtual inline void data_port_write(u16, u8) = 0;

public:

    //__attribute__((optimize("O0")))
    DiskInfo *identify(u16 dev) {
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
        ActionTable cmd {};
        cmd.cmd = ATAcmd::identify_ata;
        cmd.fill_addr = false;
        cmd.device_index = dev;
        LocalDebugInfo(cout << "acting\n";)
        DetectTable result = act(cmd);
        LocalDebugInfo(cout << "act\n";)
        if (result.status.error) return nullptr;
        for(int i = 0;result.status.busy;i++) {
            if (i > 1000) return nullptr;
            io_wait();
            result = detect(cmd.device_index);
        }
        ASMLabelMark(start_move_identify_data)
        for(u16 i = 0;i < 256;i++){
            LocalDebugInfo(cout << i;)
            id[i] = data_port_read(cmd.device_index);
        }
        auto re = new DiskInfo {};
        re->LBA_support = (identify_info.support >> 9) & 1;
        re->index_num = identify_info.LBA28_sectors;
        re->PIO_supported = identify_info.PIO_supported;
        for(int i = 0;i < 40;i++){
            re->model[i] = reinterpret_cast<char *>(identify_info.model)[i];
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
		//if (id[static_cast<u32>(ATA::ID_CFA_KEY_MGMT)] & 1)
	} else {
		/* Warn the user if the device has TPM extensions */
		//if (identify_info.cmd_set2 & 0b0001)
	}
	re->index_num = identify_info.LBA28_sectors;

    auto is_power_of_2 = [](u32 n) -> bool { return (n != 0) && ((n & (n - 1)) == 0); };
	/* get current R/W Multiple count setting */
	if ((id[47] >> 8) == 0x80 && (id[59] & 0x100)) {
		unsigned int max = id[47] & 0xFF;
		unsigned int cnt = id[59] & 0xFF;
		/* only recognize/allow powers of two here */
		if (is_power_of_2(max) && is_power_of_2(cnt)) 
        if (cnt <= max)
		re->multi_count = cnt;
	}
        if (identify_info.support & (1 << 9)) {
            re->LBA_support = 1;
            if (identify_info.cmd_set2 & (1 << 10)) {
                re->LBA_support = 2;
            }
        } else {
            /* CHS */

            /* Default translation */
            re->CHS_cylinders	= identify_info.current_cylinders;
            re->CHS_heads	= identify_info.current_heads;
            re->CHS_sectors_per_track	= identify_info.current_sectors;

            if (identify_info.fields_valid & 0b01) {
                /* Current CHS translation is valid. */
                re->CHS_cylinders = identify_info.CHS_cylinders;
                re->CHS_heads = identify_info.CHS_heads;
                re->CHS_sectors_per_track = identify_info.CHS_sectors_per_track;
            }
        }
        DebugInfo(ASMLabelMark(identify_exit_point))
        return re;
    }

    __attribute__((error("Not yet")))
    rtl::pair<DetectTable, size_t> read_DMA_LBA(void *buf, size_t size, u32 LBAl, u32 LBAh, u16 count, u16 dev) {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::read_dma;
        cmd.device_index = dev;
        cmd.LBA.low = LBAl;
        cmd.LBA.high = LBAh;
        cmd.LBA.count = count;
        cmd.buffer.addr = buf;
        cmd.buffer.size = size;
        
        DetectTable result = act(cmd);
        return {result, 0};
    }

    rtl::pair<DetectTable, size_t> read_PIO_LBA(u16 *buf, u32 LBAl, u32 LBAh, u16 count, u16 dev) {
        ActionTable cmd = {};
        cmd.cmd = (LBAh || LBAl & 0xF0000000)?ATAcmd::read_pio_ext:ATAcmd::read_pio;
        cmd.device_index = dev;
        cmd.LBA.low = LBAl;
        cmd.LBA.high = LBAh;
        cmd.LBA.count = count;
        
        DetectTable result = act(cmd);
        return PIO_read_data(buf, count, result, cmd.device_index);
    }

    rtl::pair<DetectTable, size_t> read_PIO_CHS(u16 *buf, u32 cyl, u32 head, u32 sect, u16 count, u16 dev) {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::read_pio;
        cmd.device_index = dev;
        cmd.if_need_CHS = true;
        cmd.head = head;
        cmd.CHS.cylinder = cyl;
        cmd.CHS.sector = sect;

        auto result = act(cmd);
        return PIO_read_data(buf, count, result, cmd.device_index);
    }

    rtl::pair<DetectTable, size_t> PIO_read_data(u16 *buf, u16 count, DetectTable result, u8 device_index) {
        unsigned bytes_deal = 0;
        for(u16 fan = 0;fan < count;fan++){
            unsigned i = 0;
            while (result.status.busy){
                i++;
                result = detect(device_index);
                if(i > 10000) return {result, bytes_deal};
            };
            for(unsigned int i = 0;i < 256;i++){
                buf[i + fan * 256] = data_port_read(device_index);
                bytes_deal += 2;
            }
        }
        return {detect(device_index), bytes_deal};
    }

    __attribute__((error("Not yet")))
    rtl::pair<DetectTable, size_t> write_DMA_LBA(u16 *buf, u32 LBAl, u32 LBAh, u8 count, u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::write_dma;
        cmd.device_index = dev;
        cmd.LBA.low = LBAl;
        cmd.LBA.high = LBAh;
        cmd.LBA.count = count;
        cmd.buffer.addr = buf;
        cmd.buffer.size = count * 512;
        
        DetectTable result = act(cmd);
    }

    rtl::pair<DetectTable, size_t> write_PIO_LBA(u16 *buf, u32 LBAl, u32 LBAh, u8 count, u16 dev) {
        ActionTable cmd{};
        cmd.cmd = (LBAh || LBAl & 0xF0000000)?ATAcmd::write_pio_ext:ATAcmd::write_pio_ext;
        cmd.device_index = dev;
        cmd.LBA.low = LBAl;
        cmd.LBA.high = LBAh;
        cmd.LBA.count = count;
        cmd.buffer.addr = buf;
        cmd.buffer.size = count * 512;
        
        DetectTable result = act(cmd);
        return PIO_write_data(buf, count, result, cmd.device_index);
    }

    rtl::pair<DetectTable, size_t> write_PIO_CHS(u16 *buf, u32 cyl, u32 head, u32 sect, u8 count, u16 dev) {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::write_pio;
        cmd.device_index = dev;
        cmd.if_need_CHS = true;
        cmd.head = head;
        cmd.CHS.cylinder = cyl;
        cmd.CHS.sector = sect;

        auto result = act(cmd);
        return PIO_write_data(buf, count, result, cmd.device_index);
    }

    rtl::pair<DetectTable, size_t> PIO_write_data(u16 *buf, u16 count, DetectTable result, u8 device_index) {
        unsigned bytes_deal = 0;
        for(u16 fan = 0;fan < count;fan++){
            unsigned i = 0;
            while (result.status.busy){
                i++;
                result = detect(device_index);
                if(i > 10000) return {result, bytes_deal};
            };
            for(unsigned int i = 0;i < 256;i++){
                data_port_write(buf[i + fan * 256], device_index);
                bytes_deal += 2;
            }
        }
        return {detect(device_index), bytes_deal};
    }

    DetectTable flush_cache(u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::flush_cache_ext;
        cmd.device_index = dev;
        return act(cmd);
    }

    __attribute__((error("Not yet")))
    unsigned set_features(u16 feature, u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::set_features;
        cmd.device_index = dev;
        cmd.feature = feature;
        DetectTable result = act(cmd);
    }

    DetectTable standby(u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::standby;
        cmd.device_index = dev;
        return act(cmd);
    }

    DetectTable get_smart_data(void *buf, u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::smart;
        cmd.device_index = dev;
        cmd.feature = 0xD0;  // READ DATA
        cmd.buffer.addr = buf;
        cmd.buffer.size = 512;
        return act(cmd);
    }
};
