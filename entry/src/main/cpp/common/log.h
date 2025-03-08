#ifndef HELLOXCOMPONENT_LOG_H
#define HELLOXCOMPONENT_LOG_H

#include <hilog/log.h>

#define LOG_PRINT_DOMAIN 0xFF00
#define APP_LOG_DOMAIN 0x0001
constexpr const char *APP_LOG_TAG = "HelloXComponent";

#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_DEBUG, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))

#endif //HELLOXCOMPONENT_LOG_H
