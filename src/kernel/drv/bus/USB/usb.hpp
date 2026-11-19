#ifndef __kernel_PDS_USB_usb_hpp
#define __kernel_PDS_USB_usb_hpp

#include <interface/hardev/enumer.hpp>

class USB_space : public Enumerator {
public:
    USB_space() = default;
    void probe() override;
    // USB 尚未实现，配置请求先忽略。
    void set_mod(void *) override {}
};

#endif