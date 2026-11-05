#pragma once

namespace kbd_led_ctl::posix {

class FD {
public:
  FD(int fd);

  ~FD();

  FD(const FD &) = delete;
  FD &operator=(const FD &) = delete;

  FD(FD &&other);
  FD &operator=(FD &&other);

  int get() const noexcept;
  void set(int fd);
  bool valid() const noexcept;

private:
  int fd_{-1};
};
} // namespace kbd_led_ctl::posix
