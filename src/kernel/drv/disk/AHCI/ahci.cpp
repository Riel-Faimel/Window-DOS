#include "_ahci.hpp"

void *init_SATA_controller(DeviceConfig &) {
    LocalDebugInfo(cout << "[FAILED] SATA Disk can\'t drive\n";)
    return nullptr;// new SATA_control{};
}

SATA_control::SATA_control() {
    ;
}

DetectTable SATA_control::act(const ActionTable &at) const {
    ;
}
DetectTable SATA_control::detect(DeviceIndex device_index) const {
    ;
}

rtl::pair<DetectTable, size_t> SATA_control::pio_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const {
    ;
}
rtl::pair<DetectTable, size_t> SATA_control::dma_data_in(bufAddr buf, SectorCount count, DeviceIndex device_index) const {
    ;
}
rtl::pair<DetectTable, size_t> SATA_control::pio_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const {
    ;
}
rtl::pair<DetectTable, size_t> SATA_control::dma_data_out(bufAddr buf, SectorCount count, DeviceIndex device_index) const {
    ;
}