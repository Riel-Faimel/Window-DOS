#ifndef __kernel_drv_disk_ATA_ATActl_hpp
#define __kernel_drv_disk_ATA_ATActl_hpp

#include "ATAcmd.hpp"
#include "ATAdef.hpp"
#include <interface/knlobj/diskinfo.hpp>
#include <global/type.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <stdio>

namespace ATAdev {
    
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
    u8 device_index : 5 = 0;
    u8 if_need_CHS : 1 = false;
    u8 fill_addr : 1 = true;
    u8 fill_device : 1 = true;
    u8 head : 4 = 0;
    u8 send_cmd : 1 = true;
    u8 need_48_mode : 1 = false;
    u8 force_dma : 1 = false;
    u8 re : 1 = 0;
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
    virtual rtl::pair<DetectTable, size_t> read(
        const ActionTable &, bufAddr, SectorCount
    ) const = 0;
    virtual rtl::pair<DetectTable, size_t> write(
        const ActionTable &, bufAddr, SectorCount
    ) const = 0;

    rtl::pair<DetectTable, size_t> read_lba(
        bufAddr buf, LBA lba, SectorCount count, DeviceIndex dev
    ) const {
        auto p = static_cast<u8 *>(static_cast<void *>(buf));
        unsigned sectors = static_cast<unsigned>(count.value());
        unsigned done = 0;
        DetectTable last{};

        while (sectors) {
            const bool lba48 = lba.high() || (lba.low() & 0xF0000000u);
            const unsigned limit = lba48 ? 65535u : 255u;
            const unsigned chunk = sectors > limit ? limit : sectors;

            ActionTable cmd{};
            if (lba48) {
                cmd.cmd = ATAcmd::read_pio_ext;
                cmd.need_48_mode = true;
            } else {
                cmd.cmd = ATAcmd::read_pio;
            }
            cmd.device_index = static_cast<u8>(dev);
            cmd.LBA.low = lba.low();
            cmd.LBA.high = lba.high();
            cmd.LBA.count = static_cast<u16>(chunk);

            auto [d, n] = read(cmd,
                bufAddr{static_cast<void *>(p)}, SectorCount{static_cast<u16>(chunk)}
            );
            last = d;
            if (d.status.error || d.status.busy || n == 0) break;

            done += chunk;
            sectors -= chunk;
            p += chunk * 512u;
            lba += chunk;
        }
        return {last, done * 512u};
    }

    rtl::pair<DetectTable, size_t> write_lba(
        bufAddr buf, LBA lba, SectorCount count, DeviceIndex dev
    ) const {
        auto p = static_cast<u8 *>(static_cast<void *>(buf));
        unsigned sectors = static_cast<unsigned>(count.value());
        unsigned done = 0;
        DetectTable last{};

        while (sectors) {
            const bool lba48 = lba.high() || (lba.low() & 0xF0000000u);
            const unsigned limit = lba48 ? 65535u : 255u;
            const unsigned chunk = sectors > limit ? limit : sectors;

            ActionTable cmd{};
            if (lba48) {
                cmd.cmd = ATAcmd::write_pio_ext;
                cmd.need_48_mode = true;
            } else {
                cmd.cmd = ATAcmd::write_pio;
            }
            cmd.device_index = static_cast<u8>(dev);
            cmd.LBA.low = lba.low();
            cmd.LBA.high = lba.high();
            cmd.LBA.count = static_cast<u16>(chunk);
            cmd.buffer.size = static_cast<byteCount>(chunk);

            auto [d, n] = write(cmd,
                bufAddr{static_cast<void *>(p)}, SectorCount{static_cast<u16>(chunk)}
            );
            last = d;
            if (d.status.error || d.status.busy || n == 0) break;

            done += chunk;
            sectors -= chunk;
            p += chunk * 512u;
            lba += chunk;
        }
        return {last, done * 512u};
    }

    rtl::pair<DetectTable, size_t> read_chs(
        bufAddr buf, CHS chs, SectorCount count, DeviceIndex dev
    ) const {
        DetectTable last{};
        if (!chs.sectors_per_track || !chs.heads) return {last, 0};

        auto p = static_cast<u8 *>(static_cast<void *>(buf));
        unsigned sectors = static_cast<unsigned>(count.value());
        unsigned done = 0;

        while (sectors) {
            unsigned chunk = chs.sectors_per_track - chs.sector + 1;
            if (chunk > sectors) chunk = sectors;
            if (chunk > 255u) chunk = 255u;

            ActionTable cmd{};
            cmd.cmd = ATAcmd::read_pio;
            cmd.device_index = static_cast<u8>(dev);
            cmd.if_need_CHS = true;
            cmd.head = chs.head;
            cmd.CHS.cylinder = chs.cylinder;
            cmd.CHS.sector = chs.sector;
            cmd.LBA.count = static_cast<u16>(chunk);

            auto [d, n] = read(cmd,
                bufAddr{static_cast<void *>(p)}, SectorCount{static_cast<u16>(chunk)}
            );
            last = d;
            if (d.status.error || d.status.busy || n == 0) break;

            done += chunk;
            sectors -= chunk;
            p += chunk * 512u;

            LBA next{chs};
            next += chunk;
            chs = CHS{next, chs.sectors_per_track, chs.heads};
        }
        return {last, done * 512u};
    }

    rtl::pair<DetectTable, size_t> write_chs(
        bufAddr buf, CHS chs, SectorCount count, DeviceIndex dev
    ) const {
        DetectTable last{};
        if (!chs.sectors_per_track || !chs.heads) return {last, 0};

        auto p = static_cast<u8 *>(static_cast<void *>(buf));
        unsigned sectors = static_cast<unsigned>(count.value());
        unsigned done = 0;

        while (sectors) {
            unsigned chunk = chs.sectors_per_track - chs.sector + 1;
            if (chunk > sectors) chunk = sectors;
            if (chunk > 255u) chunk = 255u;

            ActionTable cmd{};
            cmd.cmd = ATAcmd::write_pio;
            cmd.device_index = static_cast<u8>(dev);
            cmd.if_need_CHS = true;
            cmd.head = chs.head;
            cmd.CHS.cylinder = chs.cylinder;
            cmd.CHS.sector = chs.sector;
            cmd.LBA.count = static_cast<u16>(chunk);
            cmd.buffer.size = static_cast<byteCount>(chunk);

            auto [d, n] = write(cmd,
                bufAddr{static_cast<void *>(p)}, SectorCount{static_cast<u16>(chunk)}
            );
            last = d;
            if (d.status.error || d.status.busy || n == 0) break;

            done += chunk;
            sectors -= chunk;
            p += chunk * 512u;

            LBA next{chs};
            next += chunk;
            chs = CHS{next, chs.sectors_per_track, chs.heads};
        }
        return {last, done * 512u};
    }

public:

    PhysicalDiskInfo *identify(DeviceIndex dev) const {
        IdentifyAnalysis iden;

        ActionTable cmd {};
        cmd.cmd = ATAcmd::identify_ata;
        cmd.fill_addr = false;
        cmd.device_index = static_cast<u8>(dev);
        DebugInfo(ASMLabelMark(identify_acting))
#undef LocalDebug
#include <global/macro.hpp>
        auto [de, _] = read(cmd, iden, SectorCount{(u16)1u});
        if (de.status.busy || de.status.error) {
            LocalDebugInfo(kout << "No such Disk\n";)
            return nullptr;
        }
        ASMLabelMark(start_move_identify_data)

        if (iden[0] == 0xFFFF || !de.status.data_ready) {
            LocalDebugInfo(kout << "Invalid Disk\n";)
            return nullptr;
        }
        auto re = new PhysicalDiskInfo {};

        /* set disk information */
        if (auto [support, c, h, s] = iden.chs_param();support) {
            re->address_mode |= 1;
            re->CHS_cylinders = c;
            re->CHS_heads = h;
            re->CHS_sectors_per_track = s;

            re->su_num = u64{(u32)c * (u32)h * (u32)s};
        }
        if (auto [support, size28] = iden.lba_param();support) {
            re->address_mode |= 0b10;
            if (auto [support, size48] = iden.lba48_param();support) {
                re->address_mode |= 0b100;
                re->su_num = size48;
            } else {
                re->su_num = u64{size28};
            }
        }
        re->PIO_supported = iden.pio_valid();
#undef LocalDebug
#include <global/macro.hpp>
        LocalDebugInfo(kout << re->address_mode << '\n';
        if (re->PIO_supported == 0) {
            for (int i = 0;i < 256;i++) {
                print_hex(iden[i], false);print_char(' ');
            }
        })
        DebugInfo(ASMLabelMark(identify_exit_point))
        return re;
    }

    rtl::pair<DetectTable, size_t> read(
        bufAddr buf, LBA lba, SectorCount count, DeviceIndex dev
    ) const {
        return read_lba(buf, lba, count, dev);
    }

    rtl::pair<DetectTable, size_t> read(
        bufAddr buf, CHS chs, SectorCount count, DeviceIndex dev
    ) const {
        return read_chs(buf, chs, count, dev);
    }

    rtl::pair<DetectTable, size_t> write(
        bufAddr buf, LBA lba, SectorCount count, DeviceIndex dev
    ) const {
        return write_lba(buf, lba, count, dev);
    }

    rtl::pair<DetectTable, size_t> write(
        bufAddr buf, CHS chs, SectorCount count, DeviceIndex dev
    ) const {
        return write_chs(buf, chs, count, dev);
    }

    DetectTable flush_cache(u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::flush_cache_ext;
        cmd.device_index = dev;
        return act(cmd);
    }

    unsigned set_features(u16 feature, u16 dev) {
        ActionTable cmd{};
        cmd.cmd = ATAcmd::set_features;
        cmd.device_index = dev;
        cmd.feature = feature;

        DetectTable d = act(cmd);
        return (d.status.error || d.status.busy) ? (unsigned)-1 : 0u;
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

        auto [d, n] = read(cmd, bufAddr{buf}, SectorCount{(u16)1u});
        (void)n;
        return d;
    }
};

};

#endif
