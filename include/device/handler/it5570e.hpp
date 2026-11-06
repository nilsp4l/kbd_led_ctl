#pragma once

#include "hid/hid_device.hpp"

namespace kbd_led_ctl::device::handler {
void handle_it5570e(hid::HID_Device &device, uint32_t color);

}
