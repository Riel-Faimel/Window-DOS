#ifndef __kernel_drv_disk_AHCI_inner_hpp
#define __kernel_drv_disk_AHCI_inner_hpp

#include <port.h>
#include "../ATA/ATActl.hpp"
#include <interface/knlobj/knlobj.hpp>
#include <interface/hardev/drvhead.hpp>
#include <drv/bus/PCI/pci.hpp>
#include <mm/mm.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <stdio>

#pragma pack(push, 1)

// SATA 的寄存器 FIS 报文，两种方向各 20 字节。
// FIS_to   : host -> device（命令表里的 CFIS 区）
// FIS_from : device -> host（回传的状态，含 status/error）
struct FIS_to {
    u8 fis_type;   // 0x27
    u8 c_pmp;      // bit7 C | bit0-3 PMP
    u8 command;
    u8 feature_l;
    u8 lba0;
    u8 lba1;
    u8 lba2;
    u8 device;
    u8 lba3;
    u8 lba4;
    u8 lba5;
    u8 feature_h;
    u8 count_l;
    u8 count_h;
    u8 icc;
    u8 control;
    u8 rsv[4];
};

struct FIS_from {
    u8 fis_type;   // 0x27
    u8 i_pmp;      // bit7 I | bit0-3 PMP
    u8 status;
    u8 error;
    u8 lba0;
    u8 lba1;
    u8 lba2;
    u8 device;
    u8 lba3;
    u8 lba4;
    u8 lba5;
    u8 rsv0;
    u8 count_l;
    u8 count_h;
    u8 rsv1[6];
};

// 命令表头：32 字节，命令表本身 128 字节对齐
struct CommandHeader {
    u32 dw0;    // CFL:5 | A:1 | W:1 | P:1 | R:1 | B:1 | C:1 | rsv:1 | PMP:4 | rsv:4 | PRDTL:16
    u32 prdbc;  // 已传输字节数
    u32 ctba;   // 命令表物理地址低
    u32 ctbau;  // 命令表物理地址高
    u32 rsv[4];
};

// PRDT 条目：16 字节
struct PRDTEntry {
    u32 dba;   // 数据物理地址低
    u32 dbau;  // 数据物理地址高
    u32 rsv;
    u32 dbc;   // DBC:22（字节数-1） | I:1 | rsv:9
};

// 命令表：CFIS(64B) + ACMD(32B) + rsv(32B) + PRDT[]
struct CommandTable {
    FIS_to cfis;
    u8 cfis_pad[44];   // 补齐到 64 字节
    u8 acmd[32];
    u8 rsv[32];
    PRDTEntry prdt[8];
};

#pragma pack(pop)

static_assert(sizeof(FIS_to) == 20);
static_assert(sizeof(FIS_from) == 20);
static_assert(sizeof(CommandHeader) == 32);
static_assert(sizeof(PRDTEntry) == 16);
static_assert(sizeof(CommandTable) == 256);

namespace ATAdev {

class SATA_control final : public ATADiskControl, public DeviceDriver {
public:
    // Generic Host Control
    static constexpr u32 GHC_AE = 1u << 31;

    // Port 命令/中断
    static constexpr u32 PXCMD_ST  = 1u << 0;
    static constexpr u32 PXCMD_FRE = 1u << 4;
    static constexpr u32 PXCMD_FR  = 1u << 14;
    static constexpr u32 PXCMD_CR  = 1u << 15;

    static constexpr u32 PXIS_TFES  = 1u << 30;
    static constexpr u32 PXIS_TFERR = 1u << 27;
    static constexpr u32 PXIS_D2HES = 1u << 26;
    static constexpr u32 PXIS_OF    = 1u << 19;

    static constexpr u32 SSTS_DET_MASK    = 0x0F;
    static constexpr u32 SSTS_DET_PRESENT = 3;
    static constexpr u32 SIG_ATA          = 0x00000101;

private:
    volatile void *ctrl_reg_base;
    struct Global_group {
        u32 CAP;
        u32 GHC;
        u32 IS;
        u32 PI;
        u32 VS;
        u32 CCC_CHL;
        u32 CCC_PORTS;
        u32 EM_LOC;
        u32 EM_CHL;
        u32 CAP2;
        u32 BOHC;
    };
    struct Port_group {
        u32 CMD_list_base_addr;
        u32 CMD_list_base_addr_Upper_32_bits;
        u32 FIS_base_addr;
        u32 FIS_base_addr_Upper_32_bits;
        u32 IS;
        u32 IE;
        u32 CMD;
        u32 re_0;
        u32 TFD;
        u32 SIG;
        u32 SSTS;
        u32 SCHL;
        u32 SERR;
        u32 SACT;
        u32 CI;
        u32 SNTF;
        u32 FBS;
        u32 DEVSLP;
        u32 re_1;
        u32 re_2[8];   // 0x50 .. 0x6F
        u32 VS;        // 0x70
    };

    struct PortRuntime {
        u32 clb = 0;    // 命令表物理地址
        u32 fis = 0;    // FIS 接收区物理地址
        u32 ct = 0;     // 命令表头区物理地址
        bool present = false;
    };

    PCI_device_config *cfg;
    u8 port_count = 0;
    mutable PortRuntime ports[32];
    u32 bounce = 0;        // 数据中转缓冲（连续物理页）
    size_t bounce_size = 0;

    volatile Global_group &ghc() const {
        return *reinterpret_cast<volatile Global_group *>(ctrl_reg_base);
    }
    volatile Port_group &port(u8 id) const {
        // 端口寄存器从 0x100 起，每个端口占 0x80 字节（不是 0x100）。
        return *reinterpret_cast<volatile Port_group *>(
            static_cast<volatile u8 *>(ctrl_reg_base) + 0x100u + 0x80u * id
        );
    }

    bool init_controller();
    bool alloc_port(u8 id);
    void init_port(u8 id);
    void fill_fis(CommandTable &ct, const ActionTable &at) const;
    DetectTable issue(u8 id, const ActionTable &at, void *dma, size_t bytes, bool write) const;
    DetectTable wait_done(u8 id) const;
    DetectTable failed() const;

    DetectTable act(const ActionTable &at) const override final;
    DetectTable detect(DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> read(
        const ActionTable &at, bufAddr buf, SectorCount count
    ) const override final;
    rtl::pair<DetectTable, size_t> write(
        const ActionTable &at, bufAddr buf, SectorCount count
    ) const override final;
public:
    // 基类的 read/write 按地址类型重载（LBA / CHS）；override 只是机制，
    // 不写 using 会把那些重载遮住。
    using ATADiskControl::read;
    using ATADiskControl::write;

    SATA_control(DeviceInfo &info);
    /**
     * set GHC AE for AHCI(not IDE mode)
     * read PI registry to find all exist port
     *
     */
    void work() override;
    void off() override;
};

class SATA final : public PhysicalDisk {
    PhysicalDiskInfo diskinfo;
    const SATA_control &ctl;
    DeviceIndex port;

    void check();
public:
    SATA(const PhysicalDiskInfo &diskinfo_, const SATA_control &ctl_, DeviceIndex port_);

    unsigned read(void *, unsigned, unsigned, unsigned) override final;
    unsigned write(void *, unsigned, unsigned, unsigned) override final;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override final;
};

} // namespace ATAdev

#endif
