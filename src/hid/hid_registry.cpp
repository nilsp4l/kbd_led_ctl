#include "hid/hid_registry.hpp"
#include <utility>

namespace kbd_led_ctl::hid {
void HID_Registry::register_device(HID_Device &&hid_device) {
  hid_devices_.emplace_back(std::move(hid_device));
}

const std::vector<HID_Device> &HID_Registry::items() const {
  return hid_devices_;
}

} // namespace kbd_led_ctl::hid
