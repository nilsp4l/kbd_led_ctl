
#include "rgbi.hpp"
#include <array>
#include <charconv>
#include <cstddef>
#include <stdexcept>

namespace kbd_led_ctl {

// this whole thing just exists to avoid parsing integers that are bigger than
// one byte
RGBI::RGBI(const std::string &rgbi_string) {
  static constexpr std::size_t RGBI_String_Length{8};
  static constexpr std::size_t RGBI_No_Fields{4};
  if (rgbi_string.length() != RGBI_String_Length) {
    throw std::runtime_error("Bad RGBI code");
  }
  std::size_t first{0};
  std::size_t last{2};
  std::size_t channel{0};

  std::array<uint8_t *, RGBI_No_Fields> channels{&r, &g, &b, &i};

  for (; channel < RGBI_No_Fields;

       ++channel, first += 2, last += 2) {
    auto [ptr, ec]{std::from_chars(rgbi_string.c_str() + first,
                                   rgbi_string.c_str() + last,
                                   *channels[channel], 16)};
    if (ec != std::errc{} || ptr != rgbi_string.data() + last) {
      throw std::runtime_error("Bad RGBI code");
    }
  }
}

RGBI::RGBI(uint8_t r_channel, uint8_t g_channel, uint8_t b_channel,
           uint8_t i_channel)
    : r(r_channel), g(g_channel), b(b_channel), i(i_channel) {};

} // namespace kbd_led_ctl
