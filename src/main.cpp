
#include "device/device_profile.hpp"
#include "hid/hid_device.hpp"
#include "hid/hid_registry.hpp"
#include "rgbi.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <linux/hidraw.h>
#include <optional>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace kbd_led_ctl;

static void fill_registry(hid::HID_Registry &registry) {
  static constexpr const char *hidraw_dir{"/sys/class/hidraw"};
  static constexpr const char *hidraw_prefix{"hidraw"};
  static std::filesystem::path uevent_subpath{std::filesystem::path("device") /
                                              "uevent"};
  for (const auto &entry : std::filesystem::directory_iterator(hidraw_dir)) {
    std::string file_name{entry.path().filename().string()};
    if (!file_name.starts_with(hidraw_prefix)) {
      // very unlikely case but still
      continue;
    }
    std::uint32_t id;
    std::sscanf(file_name.c_str(), std::format("{}%x", hidraw_prefix).c_str(),
                &id);

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
        id, std::move(driver), kbd_led_ctl::hid::HID_ID{bus, vid, pid},
        std::move(hid_name), std::move(hid_phys), std::move(hid_uniq),
        std::move(modalias)});
  }
}

std::optional<const device::Device_Profile>
find_device_profile(const hid::HID_Device &device) {
  for (const auto &device_profile : device::Device_Profiles)
    if (device.hid_id_.vid == device_profile.Vendor_ID &&
        device.hid_id_.pid == device_profile.Product_ID) {
      return device_profile;
    }
  return {};
}

void filter_registry(hid::HID_Registry &registry) {
  registry.filter([](const hid::HID_Device &device) {
    return !find_device_profile(device).has_value();
  });
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

void list_with_profile(const hid::HID_Registry &registry) {
  for (auto &device : registry.items()) {
    auto device_profile{find_device_profile(device)};
    if (device_profile) {
      std::print("{}\n{:<20}{}\n\n", device,
                 "recognized_device:", device_profile->Name);
    } else {
      std::print("{}\n\n", device);
    }
  }
}

void find_device_filtered(uint32_t vid, uint32_t pid,
                          hid::HID_Registry &registry) {
  registry.filter([vid, pid](const hid::HID_Device &device) {
    return device.hid_id_.vid != vid || device.hid_id_.pid != pid;
  });
}

int main(int argc, char *argv[]) {
  static const option opts[] = {{"list", no_argument, nullptr, 'l'},
                                {"filtered", no_argument, nullptr, 'f'},
                                {"vid", required_argument, nullptr, 'v'},
                                {"pid", required_argument, nullptr, 'p'},
                                {"color", required_argument, nullptr, 'c'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, 0}};

  // red under full intensity

  bool list_flag{false};
  bool filtered_flag{false};
  std::optional<std::uint32_t> vid{};
  std::optional<std::uint32_t> pid{};
  std::optional<RGBI> color{};
  int c{};
  while ((c = getopt_long(argc, argv, "lfv:p:h", opts, nullptr)) != -1) {
    switch (static_cast<char>(c)) {
    case 'l':
      list_flag = true;
      break;
    case 'f':
      filtered_flag = true;
      break;
    case 'v':
      vid = from_hex(optarg);
      break;
    case 'p':
      pid = from_hex(optarg);
      break;
    case 'c':
      color = RGBI{optarg};
      break;
    case 'h':
      // usage();
      return 0;
    default:
      return 2;
    }
  }

  hid::HID_Registry registry;
  fill_registry(registry);
  if (list_flag) {
    list_with_profile(registry);
    return 0;
  }

  filter_registry(registry);

  if (filtered_flag) {
    list_with_profile(registry);
    return 0;
  }

  if (!pid || !vid) {
    return 1;
  }

  find_device_filtered(*vid, *pid, registry);

  if (registry.items().empty()) {
    std::print("No device found with vid {:#6x} and pid {:#6x}\n", *vid, *pid);
    return 0;
  }

  auto profile{find_device_profile(registry.items().front())};

  // at this point we filtered already
  assert(profile.has_value());

  auto device{registry.extract_front()};

  profile->Handler(device, color);

  return 0;
}
