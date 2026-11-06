#pragma once

#include "hid_device.hpp"
#include <functional>
#include <list>

namespace kbd_led_ctl::hid {
class HID_Registry {
public:
  HID_Registry() = default;

  void register_device(HID_Device &&hid_device);

  void filter(std::function<bool(const HID_Device &)>);

  HID_Device extract_front();

  const std::list<HID_Device> &items() const;

private:
  // we basically just iterate over all the hid devices (which arent many
  // anyway) once, focus is on filtering
  std::list<HID_Device> hid_devices_;
};
} // namespace kbd_led_ctl::hid
