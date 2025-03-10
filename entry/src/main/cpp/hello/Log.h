#ifndef HELLO_LOG_H
#define HELLO_LOG_H

#include <hilog/log.h>

#include <cstdlib>

#define LOG_PRINT_DOMAIN 0xFF00
#define APP_LOG_DOMAIN 0x0001
constexpr const char *APP_LOG_TAG = "HelloXComponent";

#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_DEBUG, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGF(...) ((void)OH_LOG_Print(LOG_APP, LOG_FATAL, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))

#define FATAL(ARGS...)  \
  { \
    LOGF(ARGS); \
    abort(); \
  }

#define FATAL_IF(CONDITION, ARGS...)  \
  if (CONDITION) { \
    FATAL(ARGS); \
  }

#define CHECK(CONDITION) \
  if (!(CONDITION)) { \
    LOGF(#CONDITION" is false"); \
    abort(); \
  }

#endif //HELLO_LOG_H
