#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <functional>
#include <string>
#include <atomic>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <queue>
#include <tuple>
#include <condition_variable>
#include "defer.hpp"
#define RET(value)  \
{                   \
    ret = value;    \
    return ret;     \
}                   \
(void)0

#define CLOSE(fd)  \
{                  \
    if (fd > 0)	{  \
        close(fd); \
        fd = -1;   \
    }              \
}                  \
(void)0

template <typename T>
std::string ToString(T v) {
  std::ostringstream oss;
  oss << v;
  return oss.str();
}

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

#define TINY_SUCCESS (0)
#define TINY_ERROR_EPOLLCREATE (-1)

#define TINY_CONTINUE (1)
#define TINY_END (2)
#define TINY_ERR (3)

namespace tiny {
using EventEnum = enum EventType_e {
    EventType_Read = 0x1,
    EventType_Write = 0x2,
    EventType_Stop = 0x4, //set this event / connection stop for long connection
    EventType_Timeout = 0x8,
};
}
