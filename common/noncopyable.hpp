#pragma once

template <unsigned value> class Noncopyable_ {
protected:
  Noncopyable_() = default;
  ~Noncopyable_() = default;
  Noncopyable_(const Noncopyable_ &) = delete;

  Noncopyable_ &operator=(const Noncopyable_ &) = delete;
};
#define Noncopyable                                                            \
private                                                                        \
  Noncopyable_<__COUNTER__>

