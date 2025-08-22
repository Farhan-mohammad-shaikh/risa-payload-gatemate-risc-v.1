#ifndef LOGGING_H_
#define LOGGING_H_

#include "serial.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void setupLogging(Serial* serial);

void log(const char* prefix, const char* file, const char* function, uint32_t line, const char* format, ...);
void print(const char* format, ...);

#define LOG_LEVEL 3


#if LOG_LEVEL > 2
#define LOGINFO(format, ...) log("INFO", __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#else
#define LOGINFO(format, ...)
#endif

#if LOG_LEVEL > 1
#define LOGTEST(format, ...) log("TEST", __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define PRINTTEST(format, ...) print(format, ##__VA_ARGS__)
#else
#define LOGTEST(format, ...)
#define PRINTTEST(format, ...)
#endif

#if LOG_LEVEL > 0
#define LOGWARN(format, ...) log("WARN", __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#else
#define LOGWARN(format, ...)
#endif

#endif // LOGGING_H_
