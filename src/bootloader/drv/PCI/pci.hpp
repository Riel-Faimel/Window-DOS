#ifndef __bootloader_drv_PCI_pci_hpp
#define __bootloader_drv_PCI_pci_hpp
#define __independent_lib_Using_template_container
#include <TL/idlib>

#pragma pack(push, 1)
struct PCI_device_config{
    //===
    u16 vender_ID;
    u16 Device_ID;
    //===
    u16 Command;
    u16 Status;
    //===
    u8 Revision_ID;
    u8 Class_code[3];
    //===
    u8 Cache_line_size;
    u8 Latency_timer;
    u8 Header_type;
    u8 BIST;
    //===
    u32 BAR[6];
    //===
    u32 Cardbus_CIS_pointer;
    //===
    u16 Subsystem_vendor_ID;
    u16 Subsystem_ID;
    //===
    u32 Expansion_ROM_base_address;
    //===
    u8 Capabilities_pointer;
    u8 re_[3];
    //===
    u32 re__;
    //===
    u8 Interrupt_line;
    u8 Interrupt_pin;
    u8 Min_grant;
    u8 Max_latency;
    //=== odinary PCI config end line ===
    void *dev_drv;
    u32 size; 
    /**
     * for only one device, it is driver object self, signed bt size = 0
     * for multiple device, it is a pointer to an array of driver objects
     * signed by size > 0
     */
};
#pragma pack(pop)

class PCI_space {
    enum class Commmand_bits {
        command_IO_space = 1 << 0,
        command_memory_space = 1 << 1,
        command_bus_master = 1 << 2,
        command_int_disable = 1 << 10
    };
    void pci_lookfor_addr(int, int, int, int);

    unsigned pci_read(int, int, int, int);

    void pci_probe_device(int, int, bool, int func_id = 0);
public:
    PCI_space(bool print_info = true);
    void set_device_driver();
    rtl::array<PCI_device_config> config{8}; // 8 is initial size, but it can be expanded if needed
};
extern const PCI_device_config *PCI_device_config_pointer; // for public
extern const unsigned PCI_device_numbers;
#endif