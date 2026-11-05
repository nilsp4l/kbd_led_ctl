
#include "hid/hid_registry.hpp"
#include "posix/fd.hpp"
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <linux/hidraw.h>
#include <optional>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace kbd_led_ctl;

static constexpr int WANTED_VENDOR_ID{0x0B05};
static constexpr int WANTED_PRODUCT_ID{0x19B6};

static std::optional<std::filesystem::path> find_hid_device() {
  static constexpr const char *hidraw_dir{"/sys/class/hidraw"};
  static constexpr const char *hidraw_prefix{"hidraw"};
  static constexpr const char *dev_prefix{"/dev"};
  static std::filesystem::path uevent_subpath{std::filesystem::path("device") /
                                              "uevent"};
  for (const auto &entry : std::filesystem::directory_iterator(hidraw_dir)) {
    if (!entry.path().filename().string().starts_with(hidraw_prefix)) {
      // very unlikely case but still
      continue;
    }

    std::ifstream uevent(entry.path() / uevent_subpath);

    if (!uevent) {
      // also quite unlikely, but still
      continue;
    }

    for (std::string line; std::getline(uevent, line);) {
      if (!line.starts_with("HID_ID="))
        continue;

      std::uint32_t bus, vid, pid;
      auto ec = std::sscanf(line.c_str(), "HID_ID=%x:%x:%x", &bus, &vid, &pid);
      // std::cout << bus << " " << vid << " " << pid << " " << ec << std::endl;
      if (ec == 3 && vid == WANTED_VENDOR_ID && pid == WANTED_PRODUCT_ID) {
        return std::filesystem::path(dev_prefix) / entry.path().filename();
      }
    }
  }
  return {};
}

int main() {

  auto path{find_hid_device()};
  if (path) {
    std::cout << path->string() << std::endl;
  }

  auto fd = posix::FD(open("/dev/hidraw1", O_RDWR));
  if (!fd.valid()) {
    perror("could not open device");
    return 1;
  }

  uint8_t cmd[2] = {0x0B, 0x01};
  if (ioctl(fd.get(), HIDIOCSFEATURE(sizeof cmd), cmd) < 0) {
    perror("HIDIOCSFEATURE");
    return 1;
  }

  uint8_t color_off[10] = {0x05, 0x01, 0x00, 0x00,       /* LampIdStart = 0 */
                           0xFF, 0x00, 0x00, 0x00, 0x00, /* R,G,B = 0 */
                           0x00};
  ioctl(fd.get(), HIDIOCSFEATURE(10), color_off);

  /*
  int size = 0;
  if (ioctl(fd, HIDIOCGRDESCSIZE, &size)) {
    perror("could not obtain desc size");
    goto error;
  }

  if (size <= 0 || size > 4096) {
    fprintf(stderr, "desc size contains unexpected value %d", size);
    goto error;
  }

  struct hidraw_report_descriptor desc;
  if (ioctl(fd, HIDIOCGRDESC, &desc)) {
    perror("could not obtain desc");
    goto error;
  }
  for (int i = 0; i < size; ++i) {
    printf("%c", desc.value[i]);
  }

  close(fd);
  return 0;
  // ioctl();
  //

error:
  close(fd);
  return 1;
  */
}
