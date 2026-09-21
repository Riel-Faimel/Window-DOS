#include "ide.hpp"
#include "..\ATA/ATActl.hpp"
#include <interface/dev_cls/device.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#include <TL/utility>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <utility>
#include <SSS/PM.hpp>
#include <intdis/intdis.hpp> 
#include <drv/screen/screen_srv.hpp>

class IDE_control final : public ATADiskControl {
	DetectTable act(const ActionTable &at) const override final;
	DetectTable detect(DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> pio_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> dma_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> pio_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> dma_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;

    static rtl::pair<u16, u8> diskID2chan_dev (DeviceIndex);

    struct Iterator {
        u8 count = 0;

        bool operator!= (const Iterator &) {
            for (;count < 4;count++) {
                auto [chan, dev] = ::IDE_control::diskID2chan_dev(DeviceIndex{count});
                outb(dev, chan + CMDBlock::device);
                io_wait();
                if(inb(chan + CMDBlock::status) == 0xFF) continue;
                else return true;
            }
            return false;
        }
        Iterator &operator++() { count++; return *this; }
        DeviceIndex operator* () { return DeviceIndex{count}; }
    };
    Iterator begin() { return Iterator{}; }
    Iterator end() { return Iterator{}; }
public:
    IDE_control();
};

class IDE final : public LogicalDisk {
    DiskInfo diskinfo;
    const IDE_control &ctl;
    DeviceIndex device;
public:
    IDE(const DiskInfo &diskinfo, const IDE_control &ctl, DeviceIndex device);
	unsigned read(void *, unsigned, unsigned, unsigned) override final;
	unsigned write(void *, unsigned, unsigned, unsigned) override final;
    DiskInfo *info(unsigned = 0, String = {}) override final;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override final;
};

extern "C" void when_PATA_Master_cut_handler();
extern "C" void when_PATA_Slave_cut_handler();