#include "hid/hid_registry.hpp"
#include "hid/hid_device.hpp"
#include <utility>

namespace kbd_led_ctl::hid {
void HID_Registry::register_device(HID_Device &&hid_device) {
  hid_devices_.emplace_back(std::move(hid_device));
}

void HID_Registry::filter(std::function<bool(const HID_Device &)> predicate) {
  hid_devices_.remove_if(predicate);
}

HID_Device HID_Registry::extract_front() {
  auto to_return{std::move(hid_devices_.front())};
  hid_devices_.pop_front();
  return to_return;
}

const std::list<HID_Device> &HID_Registry::items() const {
  return hid_devices_;
}

} // namespace kbd_led_ctl::hid
