#pragma once

#include "device/handler/it5570e.hpp"
#include "hid/hid_device.hpp"
#include <array>
#include <cstdint>
#include <print>
#include <string>

namespace kbd_led_ctl::device {
using Handler_T = void (*)(hid::HID_Device &, uint32_t color);

struct Device_Profile {
  const std::string Name;
  const std::uint32_t Vendor_ID;
  const std::uint32_t Product_ID;

  const Handler_T Handler;
};

static const Device_Profile IT5570E{"IT5570E", 0x0B05, 0x19B6,
                                    handler::handle_it5570e};

static const auto Device_Profiles{std::array{IT5570E}};
} // namespace kbd_led_ctl::device
