#include "posix/fd.hpp"
#include <unistd.h>

namespace kbd_led_ctl::posix {
FD::FD(int fd) : fd_(fd) {}

FD::~FD() {
  if (fd_ > 0) {
    close(fd_);
  }
}

FD::FD(FD &&other) {
  fd_ = other.get();
  other.set(-1);
}

FD &FD::operator=(FD &&other) {
  fd_ = other.get();
  other.set(-1);
  return *this;
}

int FD::get() const noexcept { return fd_; }

void FD::set(int fd) { fd_ = fd; }

bool FD::valid() const noexcept { return fd_ >= 0; }

} // namespace kbd_led_ctl::posix
