#ifndef __bootloader_DescrpTable_IDNT_hpp
#define __bootloader_DescrpTable_IDNT_hpp

enum class IDNT : unsigned int {
    _DE = 0,
    _OF = 4,
    _UD = 6,
    _NM = 7,
    _DF = 8,
    _NP = 11,
    _SS = 12,
    _GP = 13,
    time = 48,
    keyboard = 49,
    COM1 = 51,
    COM2 = 52,
    PS_2 = 60,
    ATA_Master = 62,
    ATA_Slave = 63,
};

#endif