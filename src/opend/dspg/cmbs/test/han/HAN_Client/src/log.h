#ifndef __LOG_H__
#define __LOG_H__
#include "TypeDefs.h"

void InitLog(bool bEnableTrace, bool bEnableError, bool bLogToScreen);

void LogTrace(const char *format, ...);

void LogError(const char *format, ...);

#define LOG_ERROR LogError
#define LOG_TRACE LogTrace

#endif

