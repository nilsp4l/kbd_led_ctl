#pragma once

#include "hid/hid_registry.hpp"
#include "rgbi.hpp"

namespace kbd_led_ctl::device::handler {
void handle_it5570e(hid::HID_Registry &registry, std::optional<RGBI> color);

}
