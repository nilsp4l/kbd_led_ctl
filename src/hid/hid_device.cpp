
#include "hid/hid_device.hpp"
#include "posix/fd.hpp"
#include "posix/posix_assert.hpp"
#include <fcntl.h>
#include <filesystem>

namespace kbd_led_ctl::hid {

HID_Device::HID_Device(ID id, DRIVER driver, HID_ID hid_id, HID_NAME hid_name,
                       HID_PHYS hid_phys, HID_UNIQ hid_uniq, MODALIAS modalias)
    : id_(id), driver_(driver), hid_id_(hid_id), hid_name_(hid_name),
      hid_phys_(hid_phys), hid_uniq_(hid_uniq), modalias_(modalias) {}

void HID_Device::open() {
  if (fd_.valid()) {
    throw std::runtime_error("Attempted double open on HID device");
  }
  static constexpr const char *dev_prefix{"/dev"};
  static constexpr const char *hidraw_prefix{"hidraw"};

  const auto path{
      std::filesystem::path{std::filesystem::path{dev_prefix} /
                            std::format("{}{}", hidraw_prefix, id_)}};
  fd_ = posix::FD{::open(path.c_str(), O_RDWR)};
  POSIX_ASSERT(fd_.valid());
}

int HID_Device::fd() const { return fd_.get(); }

bool HID_Device::opened() const { return fd_.valid(); }

} // namespace kbd_led_ctl::hid
