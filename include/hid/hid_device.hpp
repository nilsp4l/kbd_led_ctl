#pragma once

#include "posix/fd.hpp"
#include <cstdint>
#include <print>
#include <string>

namespace kbd_led_ctl::hid {

struct HID_ID {
  std::uint32_t bus, vid, pid;
};

struct HID_Device {
  using ID = int;
  using DRIVER = std::string;
  using HID_ID = hid::HID_ID;
  using HID_NAME = std::string;
  using HID_PHYS = std::string;
  using HID_UNIQ = std::string;
  using MODALIAS = std::string;

  HID_Device(ID id, DRIVER driver, HID_ID hid_id, HID_NAME hid_name,
             HID_PHYS hid_phys, HID_UNIQ hid_uniq, MODALIAS modalias);

  const ID id_;
  const DRIVER driver_;
  const HID_ID hid_id_;
  const HID_NAME hid_name_;
  const HID_PHYS hid_phys_;
  const HID_UNIQ hid_uniq_;
  const MODALIAS modalias_;

private:
  posix::FD fd_{-1};
};

} // namespace kbd_led_ctl::hid

template <> struct std::formatter<kbd_led_ctl::hid::HID_ID> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const kbd_led_ctl::hid::HID_ID &hid_id,
              std::format_context &ctx) const {
    auto out = ctx.out();
    out = std::format_to(out, "[{:#06x}:{:#06x}:{:#06x}]", hid_id.bus,
                         hid_id.vid, hid_id.pid);
    return out;
  }
};
template <> struct std::formatter<kbd_led_ctl::hid::HID_Device> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const kbd_led_ctl::hid::HID_Device &hid_device,
              std::format_context &ctx) const {
    auto out = ctx.out();
    out = std::format_to(out, "{:<20}{}\n", "name:", hid_device.hid_name_);
    out = std::format_to(out, "{:<20}{}\n", "hid id:", hid_device.hid_id_);
    out = std::format_to(out, "{:<20}{}\n", "hid_name:", hid_device.hid_name_);
    out = std::format_to(out, "{:<20}{}\n", "hid_phys:", hid_device.hid_phys_);
    out = std::format_to(out, "{:<20}{}\n", "hid_uniq:", hid_device.hid_uniq_);
    out = std::format_to(out, "{:<20}{}\n", "modalias:", hid_device.modalias_);
    return out;
  }
};
