#pragma once

#include <shlwapi.h>

#ifdef NDEBUG
    #define ASSERT(exp)
    #define DPRINTF(fmt, ...)
    #define OBJECTS_CHECK_POINT()
#else // ndef NDEBUG
    #include <assert.h>
    #define ASSERT(exp) assert(exp)
    #define DPRINTF(fmt, ...) DebugPrintf(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    static inline void
    DebugVPrintf(const char *file, int line, const char *fmt, va_list va)
    {
        CHAR buf[512];
        INT cch = wnsprintfA(buf, _countof(buf), "%s (%d): ", file, line);
        if (_countof(buf) > cch)
            wvnsprintfA(&buf[cch], _countof(buf) - cch, fmt, va);
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

    #if (_WIN32_WINNT >= 0x0500)
        #define OBJECTS_CHECK_POINT() do { \
            DPRINTF("GDI Objects: %ld, User Objects: %ld\n", \
            GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS), \
            GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS)); \
        } while (0)
    #else
        #define OBJECTS_CHECK_POINT()
    #endif
#endif // ndef NDEBUG
