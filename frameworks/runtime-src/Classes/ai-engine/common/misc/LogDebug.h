#ifndef LOGDEBUG_H
#define LOGDEBUG_H

#if defined(COCOS2D_DEBUG)
#include "cocos2d.h"

#define AILOG CCLOG

#elif defined(WIN32)
#include "stdarg.h"
#include <windows.h>
#define MAX_LOG_LEN 4096
void Trace(const char *format, ...)
{    
    char buf[MAX_LOG_LEN+1] = {0};    
    va_list args;    
    va_start(args, format);    
    vsnprintf (buf, sizeof(buf)-1, format, args);    
    va_end(args);  


    int pos = 0;
    int len = strlen(buf);
    char tempBuf[MAX_LOG_LEN + 1] = { 0 };
    WCHAR wszBuf[MAX_LOG_LEN + 1] = { 0 };
    do{
        std::copy(buf + pos, buf + pos + MAX_LOG_LEN, tempBuf);

        tempBuf[MAX_LOG_LEN] = 0;

        MultiByteToWideChar(CP_UTF8, 0, tempBuf, -1, wszBuf, sizeof(wszBuf));
        OutputDebugStringW(wszBuf);
        WideCharToMultiByte(CP_ACP, 0, wszBuf, -1, tempBuf, sizeof(tempBuf), nullptr, FALSE);

        pos += MAX_LOG_LEN;

    } while (pos < len);
    
    OutputDebugStringA("\n");
    
}

#define AILOG(format, ...) Trace(format, ##__VA_ARGS__)

#else 

#define AILOG(...)       do {} while (0)

#endif



#endif 
