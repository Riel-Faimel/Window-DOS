#ifndef __bootloader_PDS_USB_usb_hpp
#define __bootloader_PDS_USB_usb_hpp

#include <interface/enumer.hpp>

class USB_space : public Enumerator {
public:
    USB_space() = default;
    void set_driver();
    void probe() override;
};

#endif