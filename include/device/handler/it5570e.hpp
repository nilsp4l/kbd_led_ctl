#pragma once

#include "hid/hid_device.hpp"
#include "rgbi.hpp"

namespace kbd_led_ctl::device::handler {
void handle_it5570e(hid::HID_Device &device, std::optional<RGBI> color);

}
