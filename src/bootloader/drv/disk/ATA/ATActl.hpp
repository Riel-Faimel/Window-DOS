#include "ATAcmd.hpp"
#include <interface/dev_cls/cluster.hpp>
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <drv/screen/screen_srv.hpp>

#pragma pack(push, 1)
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
    u8 fill_device : 1 = true;
    u8 send_cmd : 1 = true;
    u8 need_48_mode : 1 = false;
    u8 force_dma : 1 = false;
    u8 re : 2 = 0;
    // 64 bits till here

};

struct DetectTable {
    ATAerror error;
    ATAstatus status;
};

#pragma pack(pop)

static_assert(sizeof(ActionTable) == 24);
static_assert(sizeof(DetectTable) == 5);

class ATADisk {
    virtual DetectTable act(const ActionTable &) = 0;
    virtual DetectTable detect(u8 device_index) = 0;
    virtual inline u16 data_port_read(u8) = 0;
    virtual inline void data_port_write(u16, u8) = 0;

public:

    DiskInfo *identify(u16 dev) {
        IdentifyAnalysis iden;

        ActionTable cmd {};
        cmd.cmd = ATAcmd::identify_ata;
        cmd.fill_addr = false;
        cmd.device_index = dev;
        DebugInfo(ASMLabelMark(identify_acting))
        DetectTable result = act(cmd);
        if (result.status.busy || result.status.error) {
            LocalDebugInfo(cout << "No such Disk\n";)
            return nullptr;
        }
        for (u8 i = 0;!result.status.data_ready;i++) {
            if (i > 128u) return nullptr;
            result = detect(dev);
        }
        LocalDebugInfo(cout << "result " << result.status.buf << ", " << (u32)result.error << '\n';)
        DebugInfo(ASMLabelMark(start_move_identify_data))

        for(u16 i = 0;i < 256;i++){
            iden[i] = data_port_read(cmd.device_index);
        }
        if (iden[0] == 0xFFFF) {
            LocalDebugInfo(cout << "Invalid Disk\n";)
            return nullptr;
        }
        auto re = new DiskInfo {};

        /* set disk information */
        if (auto [support, c, h, s] = iden.chs_param();support) {
            re->address_mode |= 1;
            re->CHS_cylinders = c;
            re->CHS_heads = h;
            re->CHS_sectors_per_track = s;

            re->index_num = u64{(u32)c * (u32)h * (u32)s};
        }
        if (auto [support, size28] = iden.lba_valid();support) {
            re->address_mode |= 0b10;
            if (auto [support, size48] = iden.lba48_valid();support) {
                re->address_mode |= 0b100;
                re->index_num = size48;
            } else {
                re->index_num = u64{size28};
            }
        }
        LocalDebugInfo(cout << re->address_mode << '\n';
        if (re->address_mode == 0) {
            for (int i = 0;i < 256;i++) {
                print_hex(iden[i], false);print_char(' ');
            }
        })
        //for(int i = 0;i < 40;i++){
        //    re->model[i] = reinterpret_cast<char *>(iden.model)[i];
        //}

        auto is_power_of_2 = [](u32 n) -> bool { return (n != 0) && ((n & (n - 1)) == 0); };
        /* get current R/W Multiple count setting */
        if ((iden[47] >> 8) == 0x80 && (iden[59] & 0x100)) {
            unsigned int max = iden[47] & 0xFF;
            unsigned int cnt = iden[59] & 0xFF;
            /* only recognize/allow powers of two here */
            if (is_power_of_2(max) && is_power_of_2(cnt)) 
            if (cnt <= max)
            re->multi_count = cnt;
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
        cmd.force_dma = true;
        
        DetectTable result = act(cmd);
        return {result, 0};
    }

    rtl::pair<DetectTable, size_t> read_PIO_LBA(u16 *buf, u32 LBAl, u32 LBAh, u16 count, u16 dev) {
        ActionTable cmd = {};
        if (LBAh || LBAl & 0xF0000000) {
            cmd.cmd = ATAcmd::read_pio_ext;
            cmd.need_48_mode = true;
        } else {
            cmd.cmd = ATAcmd::read_pio;
        }
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
        u8 timeout_count = 0;
        unsigned bytes_deal = 0;
        for(u16 fan = 0;fan < count;fan++){
            for (;result.status.busy || !result.status.data_ready;timeout_count++){
                result = detect(device_index);
                if(timeout_count > 128) return {result, bytes_deal};
            };
            for(u16 i = 0;i < 256;i++){
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
