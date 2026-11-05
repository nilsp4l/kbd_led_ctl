
#include "hid/hid_device.hpp"

namespace kbd_led_ctl::hid {

HID_Device::HID_Device(ID id, DRIVER driver, HID_ID hid_id, HID_NAME hid_name,
                       HID_PHYS hid_phys, HID_UNIQ hid_uniq, MODALIAS modalias)
    : id_(id), driver_(driver), hid_id_(hid_id), hid_name_(hid_name),
      hid_phys_(hid_phys), hid_uniq_(hid_uniq), modalias_(modalias) {}

} // namespace kbd_led_ctl::hid
