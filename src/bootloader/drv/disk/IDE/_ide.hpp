#include "ide.hpp"
#include "..\ATA/ATActl.hpp"
#include <interface/dev_cls/device.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#undef LocalDebug
#include <global/macro.hpp>
#include <port.h>
#include <utility>
#include <SSS/PM.hpp>
#include <intdis/intdis.hpp> 
#include <drv/screen/screen_srv.hpp>

class IDE_control final : public ATADisk {
	DetectTable act(const ActionTable &at) override final;
	DetectTable detect(u8 device_index) override final;
	u16 data_port_read(u8 device_index) override final;
	void data_port_write(u16 val, u8 device_index) override final;

    static rtl::pair<u16, u8> diskID2chan_dev (u8);

    struct Iterator {
        u8 count = 0;

        bool operator!= (const Iterator &) {
            for (;count < 4;count++) {
                auto [chan, dev] = ::IDE_control::diskID2chan_dev(count);
                outb(dev, chan + CMDBlock::device);
                io_wait();
                if(inb(chan + CMDBlock::status) == 0xFF) continue;
                else return true;
            }
            return false;
        }
        Iterator &operator++() { count++; return *this; }
        u8 operator* () { return count; }
    };
    Iterator begin() { return Iterator{}; }
    Iterator end() { return Iterator{}; }
public:
    IDE_control();
};

class IDE final : public LogicalDisk {
    DiskInfo diskinfo;
    IDE_control &ctl;
    u8 device;
public:
    IDE(DiskInfo &diskinfo, IDE_control &ctl, u8 device);
	unsigned read(void *, unsigned, unsigned, unsigned) override final;
	unsigned write(void *, unsigned, unsigned, unsigned) override final;
    DiskInfo *info(unsigned = 0, String = {}) override final;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override final;
};

extern "C" void when_PATA_Master_cut_handler();
extern "C" void when_PATA_Slave_cut_handler();