#pragma once

#include "hid_device.hpp"
#include <vector>

namespace kbd_led_ctl::hid {
class HID_Registry {
public:
  HID_Registry() = default;

  void register_device(HID_Device &&hid_device);

  const std::vector<HID_Device> &items() const;

private:
  std::vector<HID_Device> hid_devices_;
};
} // namespace kbd_led_ctl::hid
