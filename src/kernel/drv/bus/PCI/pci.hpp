#ifndef __kernel_PDS_PCI_pci_hpp
#define __kernel_PDS_PCI_pci_hpp

#include <HL/container>
#include <interface/hardev/enumer.hpp>
#include <interface/hardev/device.hpp>

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
    enum class Commmand_bits {
        command_IO_space = 1 << 0,
        command_memory_space = 1 << 1,
        command_bus_master = 1 << 2,
        command_int_disable = 1 << 10
    };

    // 设备的配置空间坐标。config 列表和 addr_l 按同一次 append 一一对应，
    // 因为 PCI_device_config 本身不带 bus/dev/func（驱动不该知道它们）。
    struct PCI_addr {
        u8 bus;
        u8 dev;
        u8 func;
    };
    size_t progif_num{};
    rtl::list<PCI_device_config> config{};
    rtl::list<PCI_addr> addr_l{};

    void pci_lookfor_addr(int, int, int, int);
    unsigned pci_read(int, int, int, int);
    void pci_write(int, int, int, int, unsigned);
    void pci_probe_device(int, int);
    pds::DeviceInfo *construc(PCI_device_config &pcf, PCI_space *enumer);
    PCI_addr *find_addr(void *cfg);
public:
    PCI_space();
    void probe() override;
    /**
     * 打开这台设备的门控：置 memory space + bus master。
     *
     * 参数是 pds::DeviceInfo::config，即 PCI_device_config 的地址。
     */
    void set_mod(void *cfg) override;
};


#endif