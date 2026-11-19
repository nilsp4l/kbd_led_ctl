#pragma once

#include "device/handler/it5570e.hpp"
#include "device/handler/roccat_vulcan_tkl.hpp"
#include "hid/hid_device.hpp"
#include "hid/hid_registry.hpp"
#include <cstdint>
#include <string>

namespace kbd_led_ctl::device {
using Handler_T = void (*)(hid::HID_Registry &, std::optional<RGBI> color);

struct Device_Profile {
  const std::string Name;
  const std::uint32_t Vendor_ID;
  const std::uint32_t Product_ID;

  const Handler_T Handler;
};

static const Device_Profile IT5570E{"IT5570E", 0x0B05, 0x19B6,
                                    handler::handle_it5570e};
static const Device_Profile RoccatVulcanTKL{"Roccat Vulcan TKL", 0x1e7d, 0x2fee,
                                            handler::handle_roccat_vulcan_tkl};

static const auto Device_Profiles{std::array{IT5570E, RoccatVulcanTKL}};
} // namespace kbd_led_ctl::device
