#ifndef __kernel_PDS_PCI_pci_hpp
#define __kernel_PDS_PCI_pci_hpp

#include <TL/container>
#include <interface/enumer.hpp>

#pragma pack(push, 1)
struct PCI_device_config {
    //===
    u16 vender_ID;
    u16 Device_ID;
    //===
    u16 Command;
    u16 Status;
    //===
    u8 Revision_ID;
    u8 prog_interface;
    u8 subclass_code;
    u8 class_code;
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
    void *driver = nullptr;
};
#pragma pack(pop)

class PCI_space : public Enumerator {
    /**
     * this type is static
     */

    enum class Commmand_bits {
        command_IO_space = 1 << 0,
        command_memory_space = 1 << 1,
        command_bus_master = 1 << 2,
        command_int_disable = 1 << 10
    };
    
    void pci_lookfor_addr(int, int, int, int);
    unsigned pci_read(int, int, int, int);
    void pci_probe_device(int, int);
public:
    PCI_space();
    void probe() override;
    rtl::list<PCI_device_config> config{};
};


#endif