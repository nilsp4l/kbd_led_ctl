#pragma once
#include <cstdint>
#include <string>
namespace kbd_led_ctl {
struct RGBI {
  std::uint8_t r, g, b, i;

  RGBI(const std::string &rgbi_string);
  RGBI(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t i);
};
} // namespace kbd_led_ctl
