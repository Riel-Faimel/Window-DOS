#include "ide.hpp"
#include "..\ATA/ATActl.hpp"
#include <interface/dev_cls/device.hpp>
#include <interface/dev_cls/logicaldisk.hpp>
#define LocalDebug
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
public:
    IDE_control();
};

class IDE final : public LogicalDisk {
    DiskInfo diskinfo;
    IDE_control &ctl;
    u8 device;
public:
    IDE(DiskInfo &diskinfo, IDE_control &ctl, bool device);
	unsigned read(void *, unsigned, unsigned, unsigned) override final;
	unsigned write(void *, unsigned, unsigned, unsigned) override final;
    DiskInfo *info(unsigned = 0, String = {}) override final;
    unsigned cmd(unsigned, String, void *argv, unsigned argc) override final;
};
