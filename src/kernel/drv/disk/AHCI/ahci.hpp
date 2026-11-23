#ifndef __kernel_drv_disk_AHCI_inner_hpp
#define __kernel_drv_disk_AHCI_inner_hpp

#include <port.h>
#include "../ATA/ATActl.hpp"
#include "ahci_def.hpp"
#include <interface/knlobj/knlobj.hpp>
#include <interface/hardev/drvhead.hpp>
#include <drv/bus/PCI/pci.hpp>
#include <mm/mm.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <KrnlObjMgr/kom.hpp>
#include <stdio>

namespace ATAdev {

class SATA_control final : public ATADiskControl, public DeviceDriver {
public:

private:
    volatile void *ctrl_reg_base;
    struct PortRuntime {
        u32 clb = 0;    // 命令表物理地址
        u32 fis = 0;    // FIS 接收区物理地址
        u32 ct = 0;     // 命令表头区物理地址
        bool present = false;
    };

    PCI_device_config *cfg;
    Enumerator *enumer = nullptr;   // 全局复位后要再走一次 set_mod（PCS）
    void *cfg_ptr = nullptr;
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
