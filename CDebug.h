#pragma once

#ifdef NDEBUG
    #define ASSERT(exp)
    #define DPRINTF(fmt, ...)
#else // ndef NDEBUG
    #include <assert.h>
    #define ASSERT(exp) assert(exp)
    #define DPRINTF(fmt, ...) DebugPrintf(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    static inline void
    DebugVPrintf(const char *file, int line, const char *fmt, va_list va)
    {
        CHAR buf[512];
        wvnsprintfA(buf, _countof(buf), fmt, va);
        buf[_countof(buf) - 1] = ANSI_NULL;
        OutputDebugStringA(buf);
    }

    static inline void
    DebugPrintf(const char *file, int line, const char *fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        DebugVPrintf(file, line, fmt, va);
        va_end(va);
    }
#endif // ndef NDEBUG
