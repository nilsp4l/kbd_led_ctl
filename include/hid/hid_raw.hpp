#pragma once

#include "hid/hid_registry.hpp"
#include <filesystem>
#include <fstream>

namespace kbd_led_ctl::hid {

template <typename T, T t> struct HID_Parsing_Handler {
  static bool operator()(HID_Device hid_device, HID_Registry &registry,
                         std::optional<HID_ID> compare);
};

struct HID_Raw {

  static constexpr const char *hidraw_dir{"/sys/class/hidraw"};
  static constexpr const char *hidraw_prefix{"hidraw"};
  static const inline std::filesystem::path uevent_subpath{
      std::filesystem::path("device") / "uevent"};

  template <typename T, T t>
  static void Parse(HID_Registry &registry,
                    std::optional<HID_ID> compare = {}) {
    for (const auto &entry : std::filesystem::directory_iterator(hidraw_dir)) {
      std::string file_name{entry.path().filename().string()};
      if (!file_name.starts_with(hidraw_prefix)) {
        // very unlikely case but still
        continue;
      }
      std::uint32_t id;
      std::sscanf(file_name.c_str(), std::format("{}%d", hidraw_prefix).c_str(),
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
      kbd_led_ctl::hid::HID_Device hid_device{
          id,
          std::move(driver),
          kbd_led_ctl::hid::HID_ID{bus, vid, pid},
          std::move(hid_name),
          std::move(hid_phys),
          std::move(hid_uniq),
          std::move(modalias)};

      if (!HID_Parsing_Handler<T, t>::operator()(std::move(hid_device),
                                                 registry, compare)) {
        break;
      }
    }
  }
};
} // namespace kbd_led_ctl::hid
