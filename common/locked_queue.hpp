#pragma once
#include "common.h"
namespace tiny {
template <typename T>
class LockedQueue {
public:
  static constexpr int MAX_QUEUE_SIZE = 20;
  explicit LockedQueue() = default;
  explicit LockedQueue(unsigned maxSize) : maxSize_(maxSize) {};
  virtual ~LockedQueue() = default;

  int Put(const T &p) {
      std::lock_guard<std::mutex> lg(qm_);
      pQueue_.push(p);
      cv_.notify_one();
  }

  T BlockedGet() {
      std::unique_lock<std::mutex> lg(qm_);
      while (pQueue_.empty()) {
          cv_.wait(lg);
      }
      T t = pQueue_.front();
      pQueue_.pop();
      return t;
  }

  std::tuple<int, T> NonblockedGet() {
      if (pQueue_.empty()) {
          return std::tuple<int, T>(-1, T(nullptr));
      } else {
          std::lock_guard<std::mutex> lg(qm_);
	  T t = pQueue_.front();
	  pQueue_.pop();
	  return std::tuple<int, T>(0, t);
      }
  } 

  int Size() { return pQueue_.size();}

  bool Empty() {
      return pQueue_.empty();
  }
private:
  std::mutex qm_;
  std::condition_variable cv_;
  std::queue<T> pQueue_;
  const unsigned maxSize_{MAX_QUEUE_SIZE};
};
}
