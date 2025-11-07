// Moji No Benkyo (3)
// Copyright (C) 2019-2025 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>

#include <vector>
#include <map>
#include <cassert>

INT g_map[80];

VOID InitMap(VOID) {
    size_t index = 0;
    g_map[index++] = 0;
    g_map[index++] = 64;
    g_map[index++] = 24;
    g_map[index++] = 27;
    g_map[index++] = 20;
    g_map[index++] = 79;
    g_map[index++] = 31;
    g_map[index++] = 69;
    g_map[index++] = 12;
    g_map[index++] = 34;
    g_map[index++] = 65;
    g_map[index++] = 73;
    g_map[index++] = 70;
    g_map[index++] = 48;
    g_map[index++] = 67;
    g_map[index++] = 17;
    g_map[index++] = 50;
    g_map[index++] = 26;
    g_map[index++] = 66;
    g_map[index++] = 43;
    g_map[index++] = 61;
    g_map[index++] = 38;
    g_map[index++] = 4;
    g_map[index++] = 16;
    g_map[index++] = 45;
    g_map[index++] = 2;
    g_map[index++] = 6;
    g_map[index++] = 41;
    g_map[index++] = 49;
    g_map[index++] = 21;
    g_map[index++] = 62;
    g_map[index++] = 63;
    g_map[index++] = 52;
    g_map[index++] = 59;
    g_map[index++] = 60;
    g_map[index++] = 40;
    g_map[index++] = 77;
    g_map[index++] = 32;
    g_map[index++] = 56;
    g_map[index++] = 36;
    g_map[index++] = 55;
    g_map[index++] = 37;
    g_map[index++] = 23;
    g_map[index++] = 1;
    g_map[index++] = 72;
    g_map[index++] = 78;
    g_map[index++] = 39;
    g_map[index++] = 7;
    g_map[index++] = 14;
    g_map[index++] = 46;
    g_map[index++] = 8;
    g_map[index++] = 19;
    g_map[index++] = 44;
    g_map[index++] = 68;
    g_map[index++] = 47;
    g_map[index++] = 25;
    g_map[index++] = 54;
    g_map[index++] = 57;
    g_map[index++] = 58;
    g_map[index++] = 10;
    g_map[index++] = 22;
    g_map[index++] = 18;
    g_map[index++] = 30;
    g_map[index++] = 5;
    g_map[index++] = 76;
    g_map[index++] = 74;
    g_map[index++] = 71;
    g_map[index++] = 51;
    g_map[index++] = 35;
    g_map[index++] = 15;
    g_map[index++] = 28;
    g_map[index++] = 75;
    g_map[index++] = 33;
    g_map[index++] = 53;
    g_map[index++] = 3;
    g_map[index++] = 9;
    g_map[index++] = 42;
    g_map[index++] = 29;
    g_map[index++] = 11;
    g_map[index++] = 13;
    assert(index == 80);

    if (0)
    {
        for (size_t i = 0; i < _countof(g_map); ++i)
        {
            g_map[i] = i;
        }
    }
    else
    {
        char buf[80];
        ZeroMemory(buf, sizeof(buf));
        for (size_t i = 0; i < _countof(g_map); ++i)
        {
            ++buf[g_map[i]];
        }
        for (size_t i = 0; i < _countof(g_map); ++i)
        {
            assert(buf[i] == 1);
        }
    }
}
