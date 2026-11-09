
#include "device/device_profile.hpp"
#include "hid/hid_device.hpp"
#include "hid/hid_raw.hpp"
#include "hid/hid_registry.hpp"
#include "rgbi.hpp"
#include <cassert>
#include <cstdint>
#include <fcntl.h>
#include <getopt.h>
#include <linux/hidraw.h>
#include <optional>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace kbd_led_ctl;

std::optional<const device::Device_Profile>
find_device_profile(const hid::HID_Device &device) {
  for (const auto &device_profile : device::Device_Profiles)
    if (device.hid_id_.vid == device_profile.Vendor_ID &&
        device.hid_id_.pid == device_profile.Product_ID) {
      return device_profile;
    }
  return {};
}

enum class Parsing_Modes {
  kFillRegistry,
  kFindViDPiD,
};

template <>
struct hid::HID_Parsing_Handler<Parsing_Modes, Parsing_Modes::kFillRegistry> {
  static bool operator()(HID_Device hid_device, HID_Registry &registry,
                         std::optional<HID_ID> compare) {
    static_cast<void>(compare);
    registry.register_device(std::move(hid_device));
    return true;
  }
};

template <>
struct hid::HID_Parsing_Handler<Parsing_Modes, Parsing_Modes::kFindViDPiD> {
  static bool operator()(HID_Device hid_device, HID_Registry &registry,
                         std::optional<HID_ID> compare) {
    if (!compare) {
      return false;
    }

    if (hid_device.hid_id_.vid == compare->vid &&
        hid_device.hid_id_.pid == compare->pid) {
      registry.register_device(std::move(hid_device));
      return false;
    }

    return true;
  }
};

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

void print_usage(std::FILE *out = stdout) {
  std::print(out, R"(kbd_led_ctl: control HID LampArray keyboard lighting

USAGE
    kbd_led_ctl --list
    kbd_led_ctl --filtered
    kbd_led_ctl --vid V --pid P [--color RRGGBBII]
    kbd_led_ctl --help

DEVICE SELECTION
    --list              enumerate all HID devices (★ = supported)
    --filtered          enumerate only devices with a profile
    --vid V --pid P     target this Vendor/Product ID (hex)

ACTIONS
    --color RRGGBBII    take host control, apply color
                        (8 hex digits; default: no color change, just switch autonomous off)
    --reset             hand control back to the firmware

SUPPORTED
    0b05:19b6           ASUS Vivobook S15 (ITE5570)

)");
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
  try {
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
        print_usage();
        return 0;
      default:
        return 2;
      }
    }
    hid::HID_Registry registry;

    if (pid && vid) {

      // bus doesnt matter for this search
      hid::HID_ID to_find{.bus = 0, .vid = *vid, .pid = *pid};
      hid::HID_Raw::Parse<Parsing_Modes, Parsing_Modes::kFindViDPiD>(registry,
                                                                     to_find);

      if (registry.items().empty()) {
        std::println("No device found with vid {:#6x} and pid {:#6x}", *vid,
                     *pid);
        return 1;
      }

      auto profile{find_device_profile(registry.items().front())};

      if (!profile) {
        std::println("Device not supported");
        return 1;
      }

      auto device{registry.extract_front()};

      profile->Handler(device, color);
      return 0;
    }

    hid::HID_Raw::Parse<Parsing_Modes, Parsing_Modes::kFillRegistry>(registry);
    if (list_flag) {
      list_with_profile(registry);
      return 0;
    }

    filter_registry(registry);

    if (filtered_flag) {
      list_with_profile(registry);
      return 0;
    }

    return 0;
  } catch (const std::system_error &e) {
    std::println(stderr, "error: {}", e.what());
    std::println(stderr, "  code: {} ({})", e.code().value(),
                 e.code().message());
    return 1;
  } catch (const std::exception &e) {
    std::println(stderr, "error: {}", e.what());
    return 1;
  } catch (...) {
    std::println(stderr, "error: unknown exception");
    return 1;
  }
}
