

#ifndef AV_SDL__SDL_LOG_H
#define AV_SDL__SDL_LOG_H

#include <stdio.h>
#include "sdl_log_config.h"

#ifdef __ANDROID__

#include <android/log.h>

#define MGLOGGER_LOG_UNKNOWN     ANDROID_LOG_UNKNOWN
#define MGLOGGER_LOG_DEFAULT     ANDROID_LOG_DEFAULT

#define MGLOGGER_LOG_VERBOSE     ANDROID_LOG_VERBOSE
#define MGLOGGER_LOG_DEBUG       ANDROID_LOG_DEBUG
#define MGLOGGER_LOG_INFO        ANDROID_LOG_INFO
#define MGLOGGER_LOG_WARN        ANDROID_LOG_WARN
#define MGLOGGER_LOG_ERROR       ANDROID_LOG_ERROR
#define MGLOGGER_LOG_FATAL       ANDROID_LOG_FATAL
#define MGLOGGER_LOG_SILENT      ANDROID_LOG_SILENT

#define VLOG(level, TAG, ...)    ((void)__android_log_vprint(level, TAG, __VA_ARGS__))
#define ALOG(level, TAG, ...)    ((void)__android_log_print(level, TAG, __VA_ARGS__))

#else

#define MGLOGGER_LOG_UNKNOWN     0
#define MGLOGGER_LOG_DEFAULT     1

#define MGLOGGER_LOG_VERBOSE     2
#define MGLOGGER_LOG_DEBUG       3
#define MGLOGGER_LOG_INFO        4
#define MGLOGGER_LOG_WARN        5
#define MGLOGGER_LOG_ERROR       6
#define MGLOGGER_LOG_FATAL       7
#define MGLOGGER_LOG_SILENT      8

#define VLOG(level, TAG, ...)    ((void)vprintf(__VA_ARGS__))
#define ALOG(level, TAG, ...)    ((void)printf(__VA_ARGS__))

#endif

#define MGLOGGER_LOG_TAG "MG_LOGGER"

#define VLOGV(...)  VLOG(MGLOGGER_LOG_VERBOSE,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#define VLOGD(...)  VLOG(MGLOGGER_LOG_DEBUG,     MGLOGGER_LOG_TAG, __VA_ARGS__)
#define VLOGI(...)  VLOG(MGLOGGER_LOG_INFO,      MGLOGGER_LOG_TAG, __VA_ARGS__)
#define VLOGW(...)  VLOG(MGLOGGER_LOG_WARN,      MGLOGGER_LOG_TAG, __VA_ARGS__)
#define VLOGE(...)  VLOG(MGLOGGER_LOG_ERROR,     MGLOGGER_LOG_TAG, __VA_ARGS__)

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_VERBOSE)
#define ALOGV(...)  ALOG(MGLOGGER_LOG_VERBOSE,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define ALOGD(...)  ALOG(MGLOGGER_LOG_DEBUG,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGD(...)
#endif

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_INFO)
#define ALOGI(...)  ALOG(MGLOGGER_LOG_INFO,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGI(...)
#endif

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_WARNING)
#define ALOGW(...)  ALOG(MGLOGGER_LOG_WARN,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGW(...)
#endif

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_ERROR)
#define ALOGE(...)  ALOG(MGLOGGER_LOG_ERROR,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGE(...)
#endif

#if (MGLOGGER_LOG_LEVEL <= LOG_LEVEL_FATAL)
#define ALOGF(...)  ALOG(MGLOGGER_LOG_FATAL,   MGLOGGER_LOG_TAG, __VA_ARGS__)
#else
#define ALOGF(...)
#endif


#endif
