#pragma once

#if DEVICE_IT5570E
#include "device/handler/it5570e.hpp"
#endif

#if DEVICE_ROCCAT_VULCAN_TKL
#include "device/handler/roccat_vulcan_tkl.hpp"
#endif

#include "hid/hid_registry.hpp"
#include "rgbi.hpp"
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

#if DEVICE_IT5570E
static const Device_Profile IT5570E{"IT5570E", 0x0B05, 0x19B6,
                                    handler::handle_it5570e};
#endif

#if DEVICE_ROCCAT_VULCAN_TKL
static const Device_Profile RoccatVulcanTKL{"Roccat Vulcan TKL", 0x1e7d, 0x2fee,
                                            handler::handle_roccat_vulcan_tkl};
#endif

static constexpr std::size_t NO_COMPILED_DEVICES{0
#if DEVICE_IT5570E
                                                 + 1
#endif

#if DEVICE_ROCCAT_VULCAN_TKL
                                                 + 1
#endif
};

inline const std::array<Device_Profile, NO_COMPILED_DEVICES> Device_Profiles{{
#if DEVICE_IT5570E
    {"IT5570E", 0x0B05, 0x19B6, handler::handle_it5570e},
#endif

#if DEVICE_ROCCAT_VULCAN_TKL
    {"Roccat Vulcan TKL", 0x1E7D, 0x2FEE, handler::handle_roccat_vulcan_tkl},
#endif
}};

} // namespace kbd_led_ctl::device
