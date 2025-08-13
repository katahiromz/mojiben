// Moji No Benkyo (3)
// Copyright (C) 2019-2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>

#include <vector>
#include <map>
#include <cassert>

#include "kakijun.h"

// Kakijun region data
KAKIJUN g_digits_kakijun;

#define ADD_LINEAR(angle, data) do { \
    ga.type     = LINEAR; \
    ga.angle0   = angle; \
    ga.cb       = sizeof(data); \
    ga.pb       = data; \
    vga.push_back(ga); \
} while (0)

#define ADD_POLAR(a0, a1, center_x, center_y, data) do { \
    ga.type     = POLAR; \
    ga.angle0   = a0; \
    ga.angle1   = a1; \
    ga.cx       = center_x; \
    ga.cy       = center_y; \
    ga.cb       = sizeof(data); \
    ga.pb       = data; \
    vga.push_back(ga); \
} while (0)

#define ADD_WAIT() do { \
    ga.type     = WAIT; \
    vga.push_back(ga); \
} while (0)

VOID InitDigits(VOID)
{
    GA ga;
    std::vector<GA> vga;
    size_t index = 0;
    vga.clear();
}
