#include "_ahci.hpp"

void *init_SATA_controller(DeviceConfig &) {
    return new SATA_control{};
}

SATA_control::SATA_control() {
    ;
}

DetectTable SATA_control::act(const ActionTable &) const {}

DetectTable SATA_control::detect(u8) const {}

size_t SATA_control::pio_data_in(u16 *buf, size_t size, u8 device_index) const {
    ;
}
size_t SATA_control::dma_data_in(u16 *buf, size_t size, u8 device_index) const {
    ;
}
size_t SATA_control::pio_data_out(u16 *buf, size_t size, u8 device_index) const {
    ;
}
size_t SATA_control::dma_data_out(u16 *buf, size_t size, u8 device_index) const {
    ;
}