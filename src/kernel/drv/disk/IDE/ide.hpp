#include "..\ATA/ATActl.hpp"
#include <interface/hardev/device.hpp>
#include <interface/knlobj/knlobj.hpp>
#include <interface/hardev/drvhead.hpp>
#include <HL/utility>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <utility>
#include <id/id.hpp> 
#include <stdio>
#include <interface/hardev/pift.hpp>
#include <KOM/kom.hpp>

namespace ATAdev {

class IDE_control final : public ATADiskControl, public DeviceDriver {
	DetectTable act(const ActionTable &at) const override final;
	DetectTable detect(DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> pio_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> dma_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> pio_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;
    rtl::pair<DetectTable, size_t> dma_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const override final;

    static rtl::pair<u16, u8> diskID2chan_dev (DeviceIndex);

    struct iterator {
        u8 count = 0;

        bool operator!= (const iterator &) {
            for (;count < 4;count++) {
                auto [chan, dev] = IDE_control::diskID2chan_dev(DeviceIndex{count});
                outb(dev, chan + CMDBlock::device);
                io_wait();
                if(inb(chan + CMDBlock::status) == 0xFF) continue;
                else return true;
            }
            return false;
        }
        iterator &operator++() { count++; return *this; }
        DeviceIndex operator* () { return DeviceIndex{count}; }
    };
    iterator begin() { return iterator{}; }
    iterator end() { return iterator{}; }
public:
    IDE_control();

    void work() override;
    void off() override;
};

class IDE final : public PhysicalDisk {
    PhysicalDiskInfo diskinfo;
    const IDE_control &ctl;
    DeviceIndex device;
public:
    IDE(const PhysicalDiskInfo &diskinfo, const IDE_control &ctl, DeviceIndex device);
	unsigned read(void *, unsigned, unsigned, unsigned) override final;
	unsigned write(void *, unsigned, unsigned, unsigned) override final;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override final;
};
    
} // namespace ATAdev 

extern "C" void when_PATA_Master_cut_handler();
extern "C" void when_PATA_Slave_cut_handler();