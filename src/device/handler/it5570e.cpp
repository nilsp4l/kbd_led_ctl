#include "device/handler/it5570e.hpp"
#include "posix/posix_assert.hpp"
#include <cstddef>
#include <linux/hidraw.h>
#include <sys/ioctl.h>

namespace kbd_led_ctl::device::handler {

#define AUTONOMOUS 0x0B
#define OFF 0x00
#define ON 0x01
#define AUTONOMOUS_OFF AUTONOMOUS, OFF
#define AUTONOMOUS_ON AUTONOMOUS, ON

/*
 * Wire layout of the LampRangeUpdate report (for reference only):
 *
 *   [0]   report ID        0x05
 *   [1]   LampUpdateFlags  0x01 (LampUpdateComplete)
 *   [2-3] LampIdStart      little-endian
 *   [4-5] LampIdEnd        little-endian
 *   [6]   Red
 *   [7]   Green
 *   [8]   Blue
 *   [9]   Intensity
 */

void handle_it5570e(hid::HID_Device &device, uint32_t color) {

  if (!device.opened()) {
    device.open(); // throws
  }

  static constexpr std::size_t Autonomous_Report_Size{2};
  static constexpr std::size_t Lamp_Range_Update_Report_Size{10};

  // it has been experienced, that after boot the controller doesnt react
  // to turning autonomous off. This has been fixed by actively turning
  // autonomous on ourselves. its quite foggy why that is the case and i couldnt
  // find any docu, but here we are
  // we can probably assume that the state the controller is in at boot is
  // somewhat undefined, which can also be the whole reason we have to do this
  // in the first place. we can do so because turning on autonomous gives
  // control over brightness again
  std::array<uint8_t, Autonomous_Report_Size> autonomous_on = {AUTONOMOUS_ON};
  POSIX_ASSERT(ioctl(device.fd(), HIDIOCSFEATURE(std::size(autonomous_on)),
                     autonomous_on.data()) >= 0);

  // we want to keep our control.
  std::array<uint8_t, Autonomous_Report_Size> autonomous_off = {AUTONOMOUS_OFF};
  POSIX_ASSERT(ioctl(device.fd(), HIDIOCSFEATURE(std::size(autonomous_off)),
                     autonomous_off.data()) >= 0);

  /*
     see the struct lamp_update_flags to understand the fields

     we assume the lamp count to be 1. that is a fair assumption because we
     implement for one controller explicitally here. otherwise we would need to
     send requests to find the lamp count, or set it to an arbitrary high number
     like 256 which will work anyhow on almost all devices, but that has
     performance implications for small gain.
  */
  std::array<uint8_t, Lamp_Range_Update_Report_Size> lamp_range_update_report =
      {0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x1b, 0x2c, 0xFF};
  POSIX_ASSERT(ioctl(device.fd(),
                     HIDIOCSFEATURE(std::size(lamp_range_update_report)),
                     lamp_range_update_report.data()) >= 0);
}

} // namespace kbd_led_ctl::device::handler
