#include "sata.hpp"
#include <lib/hardlib/x86/port.h>

struct FIS_to {};

struct FIS_from {};

class STAT_controller {
    volatile void *ctrl_reg_base;
    struct Global_group {
        u32 CAP;
        u32 GHC;
        u32 IS;
        u32 PI;
        u32 VS;
        u32 CCC_CTL;
        u32 CCC_PORTS;
        u32 EM_LOC;
        u32 EM_CTL;
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
        u32 SCTL;
        u32 SERR;
        u32 SACT;
        u32 CI;
        u32 SNTF;
        u32 FBS;
        u32 DEVSLP;
        u32 re_1;
        u32 VS;
    };


    STAT_controller(){}
    /**
     * set GHC AE for AHCI(not IDE mode)
     * read PI registry to find all exist port
     * 
     */
};