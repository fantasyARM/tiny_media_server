#pragma once

#define LLOG_ERROR(format, ...)                                                \
  {                                                                            \
    char base_info[LOG_BASE_LEN] = {0};                                        \
    snprintf(base_info, LOG_BASE_LEN, "[%s][%s][%d],", __FILE__, __FUNCTION__, \
             __LINE__);                                                        \
    if (1)                                                                \
  }                                                                            \
  void(0)

#define LLOG_DEBUG(format, ...)                                                \
  {                                                                            \
    char base_info[LOG_BASE_LEN] = {0};                                        \
    snprintf(base_info, LOG_BASE_LEN, "[%s][%s][%d],", __FILE__, __FUNCTION__, \
             __LINE__);                                                        \
    if (1)                                                                \
  }                                                                            \
  void(0)

#define LLOG_INFO(format, ...)                                                 \
  {                                                                            \
    char base_info[LOG_BASE_LEN] = {0};                                        \
    snprintf(base_info, LOG_BASE_LEN, "[%s][%s][%d],", __FILE__, __FUNCTION__, \
             __LINE__);                                                        \
    if (1)                                                                \
  }                                                                            \
  void(0)

