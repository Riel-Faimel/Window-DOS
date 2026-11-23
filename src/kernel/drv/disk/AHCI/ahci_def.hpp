#pragma once

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


    // Generic Host Control
static constexpr u32 GHC_AE = 1u << 31;
// Port 命令/中断

static constexpr u32 PXCMD_ST  = 1u << 0;
static constexpr u32 PXCMD_SUD = 1u << 1;   // spin-up device
static constexpr u32 PXCMD_POD = 1u << 2;   // power on device
static constexpr u32 PXCMD_FRE = 1u << 4;
static constexpr u32 PXCMD_FR  = 1u << 14;
static constexpr u32 PXCMD_CR  = 1u << 15;
// 接口电源状态：0=idle，1=active，2=partial，6=slumber。
// BIOS 可能把端口留在 partial/slumber；不写 ACTIVE 链路不会起来。
static constexpr u32 PXCMD_ICC_MASK   = 0xFu << 28;
static constexpr u32 PXCMD_ICC_ACTIVE = 1u << 28;

static constexpr u32 PXIS_TFES  = 1u << 30;
static constexpr u32 PXIS_TFERR = 1u << 27;
static constexpr u32 PXIS_D2HES = 1u << 26;
static constexpr u32 PXIS_OF    = 1u << 19;

static constexpr u32 SSTS_DET_MASK    = 0x0F;
static constexpr u32 SSTS_DET_PRESENT = 3;
static constexpr u32 SIG_ATA          = 0x00000101;