#pragma once

#define POSIX_ASSERT(expr)                                                     \
  do {                                                                         \
    if (!(expr)) {                                                             \
      int _e = errno;                                                          \
      throw std::system_error(std::error_code{_e, std::system_category()},     \
                              #expr);                                          \
    }                                                                          \
  } while (0)
