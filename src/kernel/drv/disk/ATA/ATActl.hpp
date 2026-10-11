#ifndef __kernel_drv_disk_ATA_ATActl_hpp
#define __kernel_drv_disk_ATA_ATActl_hpp

#include "ATAcmd.hpp"
#include "ATAdef.hpp"
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
        byteCount size = byteCount{0};
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

class ATADiskControl {
    virtual DetectTable act(const ActionTable &) const = 0;
    virtual DetectTable detect(DeviceIndex device_index) const = 0;
    virtual rtl::pair<DetectTable, size_t> pio_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const = 0;
    virtual rtl::pair<DetectTable, size_t> dma_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const = 0;
    virtual rtl::pair<DetectTable, size_t> pio_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const = 0;
    virtual rtl::pair<DetectTable, size_t> dma_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const = 0;

public:

    DiskInfo *identify(DeviceIndex dev) const {
        IdentifyAnalysis iden;

        ActionTable cmd {};
        cmd.cmd = ATAcmd::identify_ata;
        cmd.fill_addr = false;
        cmd.device_index = static_cast<u8>(dev);
        DebugInfo(ASMLabelMark(identify_acting))
#undef LocalDebug
#include <global/macro.hpp>
        DetectTable result = act(cmd);
        if (result.status.busy || result.status.error) {
            LocalDebugInfo(cout << "No such Disk\n";)
            return nullptr;
        }
        ASMLabelMark(start_move_identify_data)

        auto [de, _] = pio_data_in(iden, SectorCount{(u16)1u}, DeviceIndex{cmd.device_index});
        if (iden[0] == 0xFFFF || !de.status.data_ready) {
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
        if (auto [support, size28] = iden.lba_param();support) {
            re->address_mode |= 0b10;
            if (auto [support, size48] = iden.lba48_param();support) {
                re->address_mode |= 0b100;
                re->index_num = size48;
            } else {
                re->index_num = u64{size28};
            }
        }
        re->PIO_supported = iden.pio_valid();
#undef LocalDebug
#include <global/macro.hpp>
        LocalDebugInfo(cout << re->address_mode << '\n';
        if (re->PIO_supported == 0) {
            for (int i = 0;i < 256;i++) {
                print_hex(iden[i], false);print_char(' ');
            }
        })
        DebugInfo(ASMLabelMark(identify_exit_point))
        return re;
    }

    __attribute__((error("Not yet")))
    rtl::pair<DetectTable, size_t> read_DMA_LBA(bufAddr buf, LBAaddr LBA, SectorCount count, DeviceIndex dev) const {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::read_dma;
        cmd.device_index = static_cast<u8>(dev);
        cmd.LBA.low = LBA.low;
        cmd.LBA.high = LBA.high;
        cmd.LBA.count = static_cast<u16>(count);
        cmd.buffer.addr = static_cast<void *>(buf);
        cmd.buffer.size = static_cast<byteCount>(count);
        cmd.force_dma = true;
        
        act(cmd);
        return dma_data_in(buf, count, dev);
    }

    rtl::pair<DetectTable, size_t> read_PIO_LBA(bufAddr buf, LBAaddr LBA, SectorCount count, DeviceIndex dev) const {
        #undef LocalDebug
        #include <global/macro.hpp>
        LocalDebugInfo(
            cout << '[' << LBA.high;print_hex(LBA.low, false);cout << ", " << static_cast<u16>(count) << "]\n";
        )
        ActionTable cmd = {};
        if (LBA.high || LBA.low & 0xF0000000) {
            cmd.cmd = ATAcmd::read_pio_ext;
            cmd.need_48_mode = true;
        } else {
            cmd.cmd = ATAcmd::read_pio;
        }
        cmd.device_index = static_cast<u8>(dev);
        cmd.LBA.low = LBA.low;
        cmd.LBA.high = LBA.high;
        cmd.LBA.count = static_cast<u16>(count);
        
        act(cmd);
        return pio_data_in(buf, count, dev);
    }

    rtl::pair<DetectTable, size_t> read_PIO_CHS(bufAddr buf, u32 cyl, u32 head, u32 sect, SectorCount count, DeviceIndex dev) const {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::read_pio;
        cmd.device_index = static_cast<u8>(dev);
        cmd.if_need_CHS = true;
        cmd.head = head;
        cmd.CHS.cylinder = cyl;
        cmd.CHS.sector = sect;

        act(cmd);
        return pio_data_in(bufAddr{buf}, SectorCount{count}, dev);
    }

    __attribute__((error("Not yet")))
    DetectTable write_DMA_LBA(bufAddr buf, LBAaddr LBA, SectorCount count, DeviceIndex dev) const {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::write_dma;
        cmd.device_index = static_cast<u8>(dev);
        cmd.LBA.low = LBA.low;
        cmd.LBA.high = LBA.high;
        cmd.LBA.count = static_cast<u16>(count);
        cmd.buffer.addr = static_cast<void *>(buf);
        cmd.buffer.size = static_cast<byteCount>(count);
        
        return act(cmd);
    }

    rtl::pair<DetectTable, size_t> write_PIO_LBA(bufAddr buf, LBAaddr LBA, SectorCount count, DeviceIndex dev) const {
        ActionTable cmd{};
        cmd.cmd = (LBA.high || LBA.low & 0xF0000000)?ATAcmd::write_pio_ext:ATAcmd::write_pio_ext;
        cmd.device_index = static_cast<u8>(dev);
        cmd.LBA.low = LBA.low;
        cmd.LBA.high = LBA.high;
        cmd.LBA.count = static_cast<u16>(count);
        cmd.buffer.size = static_cast<byteCount>(count);
        
        act(cmd);
        return pio_data_out(buf, count, dev);
    }

    rtl::pair<DetectTable, size_t> write_PIO_CHS(bufAddr buf, u32 cyl, u32 head, u32 sect, SectorCount count, DeviceIndex dev) const {
        ActionTable cmd = {};
        cmd.cmd = ATAcmd::write_pio;
        cmd.device_index = static_cast<u8>(dev);
        cmd.if_need_CHS = true;
        cmd.head = head;
        cmd.CHS.cylinder = cyl;
        cmd.CHS.sector = sect;

        act(cmd);
        return pio_data_out(buf, count, dev);
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
        cmd.buffer.size = static_cast<byteCount>(512);
        return act(cmd);
    }
};

#endif