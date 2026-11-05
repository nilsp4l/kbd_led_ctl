
#include "hid/hid_device.hpp"
#include "hid/hid_registry.hpp"
#include "posix/fd.hpp"
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <linux/hidraw.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace kbd_led_ctl;

static constexpr int WANTED_VENDOR_ID{0x0B05};
static constexpr int WANTED_PRODUCT_ID{0x19B6};

#define BUFFER_SIZE 256;

static void list(hid::HID_Registry &registry) {
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

    std::uint32_t bus, vid, pid;

    std::string driver{};
    std::string hid_name;
    std::string hid_phys{};
    std::string hid_uniq{};
    std::string modalias{};

    for (std::string line; std::getline(uevent, line);) {
      if (line.starts_with("HID_ID=")) {
        auto ec =
            std::sscanf(line.c_str(), "HID_ID=%x:%x:%x", &bus, &vid, &pid);

        if (ec != 3) {
          return;
        }

        continue;
      }

      std::array<std::string, 2> key_value;
      auto eq{line.find('=')};
      if (eq == std::string_view::npos) {
        return;
      }

      key_value = {std::string{line.substr(0, eq)},
                   std::string{line.substr(eq + 1)}};

      if (key_value[0] == "DRIVER") {
        driver = std::move(key_value[1]);
      } else if (key_value[0] == "HID_NAME") {
        hid_name = std::move(key_value[1]);
      } else if (key_value[0] == "HID_PHYS") {
        hid_phys = std::move(key_value[1]);
      } else if (key_value[0] == "HID_UNIQ") {
        hid_uniq = std::move(key_value[1]);
      } else if (key_value[0] == "MODALIAS") {
        modalias = std::move(key_value[1]);
      }
    }
    registry.register_device(kbd_led_ctl::hid::HID_Device{
        0, std::move(driver), kbd_led_ctl::hid::HID_ID{bus, vid, pid},
        std::move(hid_name), std::move(hid_phys), std::move(hid_uniq),
        std::move(modalias)});
  }
}

uint32_t from_hex(const char *str) {
  std::string_view hex_str{str};
  if (hex_str.starts_with("0x")) {
    hex_str.remove_prefix(2);
  }

  uint32_t to_return{};
  auto [ptr, ec]{std::from_chars(
      hex_str.data(), hex_str.data() + hex_str.size(), to_return, 16)};
  if (ec == std::errc{} && ptr != hex_str.data() + hex_str.size()) {
    return 0;
  }

  return to_return;
}

int main(int argc, char *argv[]) {
  static const option opts[] = {{"list", no_argument, nullptr, 'l'},
                                {"filtered", no_argument, nullptr, 'f'},
                                {"vid", required_argument, nullptr, 'v'},
                                {"pid", required_argument, nullptr, 'p'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, 0}};

  bool list_flag{false};
  bool filtered_flag{false};
  bool have_vid{false};
  bool have_pid{false};
  std::uint32_t vid{};
  std::uint32_t pid{};
  int c{};
  while ((c = getopt_long(argc, argv, "lfv:p:h", opts, nullptr))) {
    switch (static_cast<char>(c)) {
    case 'l':
      list_flag = true;
      break;
    case 'f':
      filtered_flag = true;
      break;
    case 'v':
      vid = from_hex(optarg);
      have_vid = true;
      break;
    case 'p':
      pid = from_hex(optarg);
      have_pid = true;
      break;
    case 'h':
      // usage();
      return 0;
    default:
      return 2;
    }
  }

  std::print("{}, {}\n", vid, pid);
  hid::HID_Registry registry;
  /*
  list(registry);
  for (auto &device : registry.items()) {
    std::print("{}\n", device);
  }
  */
  /*
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

    uint8_t color_off[10] = {0x05, 0x01, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0x00,
  0x00
};
ioctl(fd.get(), HIDIOCSFEATURE(10), color_off);
*/
  /*
    if (ec == 3 && vid == WANTED_VENDOR_ID && pid == WANTED_PRODUCT_ID) {
      return std::filesystem::path(dev_prefix) / entry.path().filename();
    }
    */
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
