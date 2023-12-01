#pragma once
#include <functional>
#include <stdio.h>
#include <string>
#include <unistd.h>

class Defer_ final {
public:
  Defer_() noexcept = default;
  ~Defer_() noexcept {
    if (f_ != nullptr) {
      f_();
    }
  }

  Defer_(Defer_ &&rhs) noexcept : f_(std::move(rhs.f_)) {}

  template <class F> void operator+(F &&f) { f_ = std::forward<F>(f); }

public:
  std::function<void()> f_;
};

#define DEFER_CREATE_NAME2(x, y) x##y
#define DEFER_CREATE_NAME1(F, ...) F(__VA_ARGS__)
#define DEFER_CREATE_NAME                                                      \
  DEFER_CREATE_NAME1(DEFER_CREATE_NAME2, __FUNCTION__, __LINE__)
#define defer                                                                  \
  Defer_ DEFER_CREATE_NAME{};                                                  \
  DEFER_CREATE_NAME +

