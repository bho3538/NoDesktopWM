#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

void XTraceW(INT flags, LPCSTR file, LPCSTR function, INT line, LPCWSTR message, ...);

#ifndef EDM_LOG_ENUM
#define EDM_LOG_ENUM
#define EDM_LOG_USEDEBUGVIEW 1024
#define EDM_LOG_INCLUDEFILEPATH 2048
#endif

#ifndef XTrace
#define XTrace(message,...) XTraceW(3072,__FILE__,__FUNCTION__,__LINE__,message,__VA_ARGS__);
#endif