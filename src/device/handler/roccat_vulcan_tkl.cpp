#include "device/handler/roccat_vulcan_tkl.hpp"
#include "hid/hid_device.hpp"
#include "hid/hid_registry.hpp"
#include "posix/posix_assert.hpp"
#include "rgbi.hpp"
#include <assert.h>
#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

namespace kbd_led_ctl::device::handler {

template <std::size_t N>
void set_feature(int fd, std::array<uint8_t, N> &request) {
  POSIX_ASSERT(ioctl(fd, HIDIOCSFEATURE(N), request.data()) >= 0);
}

template <std::size_t N>
void get_feature(int fd, std::array<uint8_t, N> &request) {
  POSIX_ASSERT(ioctl(fd, HIDIOCGFEATURE(N), request.data()) >= 0);
}

static constexpr auto NO_MAX_POLLS{100};

void sleep_for_ready(int ctrl_fd) {
  for (int i = 0; i < NO_MAX_POLLS; i++) {
    if (i) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(25ms);
    }
    std::array<uint8_t, 3> status{0x04, 0x00, 0x00};
    get_feature(ctrl_fd, status);
    if (status[1] == 0x01)
      return;
  }
  throw std::runtime_error("device not ready after 100 polls");
}

constexpr std::size_t NO_COLUMNS = 12;
constexpr std::size_t MODE_BUF_LEN = 443;

// for whatever reason roccat has this weird packet format of
// 12x(12xr,12xr,12xr) maybe thats the standard elsewhere too, but then its a
// stupid standard there as well
template <std::size_t N>
void fill_grid(std::array<uint8_t, N> &buf, std::size_t offset, RGBI color) {
  for (std::size_t col = 0; col < NO_COLUMNS; col++) {
    for (std::size_t i = 0; i < 12; i++)
      buf[offset++] = color.r;
    for (std::size_t i = 0; i < 12; i++)
      buf[offset++] = color.g;
    for (std::size_t i = 0; i < 12; i++)
      buf[offset++] = color.b;
  }
}

static constexpr uint8_t BRIGHTNESS_MAX{0x45};
static constexpr uint8_t BRIGHTNESS_MIN{0x01};

// https://stackoverflow.com/questions/12931115/algorithm-to-map-an-interval-to-a-smaller-interval
uint8_t map_intensity(uint8_t intensity) {
  int A{0x00};
  int B{0xFF};
  int a{BRIGHTNESS_MIN};
  int b{BRIGHTNESS_MAX};
  return static_cast<uint8_t>((intensity - A) * (b - a) / (B - A) + a);
}

// we set the mode to 1 because we imply that we stay on that mode. of course
// anyone can just change that and recompile. but the functionality should stay
// simple here speed doesnt matter for us because we are static
void send_mode_packet(int ctrl_fd, RGBI color, uint8_t mode = 0x01,
                      uint8_t speed = 0x01) {
  uint8_t brightness{map_intensity(color.i)};
  std::array<uint8_t, MODE_BUF_LEN> buf{};

  // clang-format off
  //
  // offset (in bytes)  0           1                     2             3   4     5
  //                    report ID   size of the buffer (little endian)  00  mode  speed
  //
  //  6       7             8       9           ...   440         441   442
  //  05      brightness    00      grid start        grid_end    checksum (little endian)
  // clang-format on 

  buf[0] = 0x0D;
  buf[1] = MODE_BUF_LEN & 0xFF;
  buf[2] = MODE_BUF_LEN >> 8;
  buf[4] = mode;
  buf[5] = speed;
  buf[6] = 0x05;
  buf[7] = brightness;
  static constexpr auto GRID_START{9};
  fill_grid(buf, GRID_START, color);

  uint16_t sum = 0;
  for (std::size_t i = 0; i < MODE_BUF_LEN - 2; i++)
    sum += buf[i];
  buf[MODE_BUF_LEN - 2] = static_cast<uint8_t>(sum & 0xFF);
  buf[MODE_BUF_LEN - 1] = static_cast<uint8_t>(sum >> 8);

  set_feature(ctrl_fd, buf);
}

void handle_device(hid::HID_Device &ctrl_device, std::optional<RGBI> color) {
  if (!ctrl_device.opened()) {
    ctrl_device.open(); // throws
  }

  send_mode_packet(ctrl_device.fd(),
                   color.value_or(RGBI{0x00, 0xff, 0x00, 0xff}));
}

void handle_roccat_vulcan_tkl(hid::HID_Registry &registry,
                              std::optional<RGBI> color) {
  for (auto &hid_device : registry) {
    // the control device will be exposed as input1
    if (hid_device.hid_phys_.contains("input1")) {
      handle_device(hid_device, color);
    }
  }
}
} // namespace kbd_led_ctl::device::handler
