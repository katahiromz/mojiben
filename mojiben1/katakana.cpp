// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>

#include <vector>
#include <map>

#include "kakijun.h"

// Kakijun region data
KAKIJUN g_katakana_kakijun;

#define ADD_LINEAR(angle, data) do { \
    ga.type     = LINEAR; \
    ga.angle0   = angle; \
    ga.res      = (200 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_POLAR(a0, a1, center_x, center_y, data) do { \
    ga.type     = POLAR; \
    ga.angle0   = a0; \
    ga.angle1   = a1; \
    ga.cx       = (center_x * 254) / 300; \
    ga.cy       = (center_y * 254) / 300; \
    ga.res      = (200 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_WAIT() do { \
    ga.type     = WAIT; \
    vga.push_back(ga); \
} while (0)

#define NEXT_MOJI() do { \
    g_katakana_kakijun.push_back(vga); \
    vga.clear(); \
    ++j; i = 0; \
} while (0)

VOID InitKatakana(VOID)
{
    STROKE ga;
    std::vector<STROKE> vga;
    size_t index = 0;
    INT j = 0, i = 0;

    ADD_LINEAR(0, a0);
    ADD_LINEAR(130, a1);
    ADD_WAIT();
    ADD_LINEAR(110, a2);

    NEXT_MOJI();

    ADD_LINEAR(140, i0);
    ADD_WAIT();
    ADD_LINEAR(90, i1);

    NEXT_MOJI();

    ADD_LINEAR(90, u0);
    ADD_WAIT();
    ADD_LINEAR(90, u1);
    ADD_WAIT();
    ADD_LINEAR(0, u2);
    ADD_LINEAR(120, u3);

    NEXT_MOJI();

    ADD_LINEAR(0, e0);
    ADD_WAIT();
    ADD_LINEAR(90, e1);
    ADD_WAIT();
    ADD_LINEAR(0, e2);

    NEXT_MOJI();

    ADD_LINEAR(0, o0);
    ADD_WAIT();
    ADD_LINEAR(90, o1);
    ADD_LINEAR(210, o2);
    ADD_WAIT();
    ADD_LINEAR(130, o3);

    NEXT_MOJI();

    ADD_LINEAR(0, ka0);
    ADD_LINEAR(110, ka1);
    ADD_LINEAR(210, ka2);
    ADD_WAIT();
    ADD_LINEAR(120, ka3);

    NEXT_MOJI();

    ADD_LINEAR(10, ki0);
    ADD_WAIT();
    ADD_LINEAR(10, ki1);
    ADD_WAIT();
    ADD_LINEAR(85, ki2);

    NEXT_MOJI();

    ADD_LINEAR(140, ku0);
    ADD_WAIT();
    ADD_LINEAR(0, ku1);
    ADD_LINEAR(130, ku2);

    NEXT_MOJI();

    ADD_LINEAR(120, ke0);
    ADD_WAIT();
    ADD_LINEAR(0, ke1);
    ADD_WAIT();
    ADD_LINEAR(120, ke2);

    NEXT_MOJI();

    ADD_LINEAR(0, ko0);
    ADD_LINEAR(90, ko1);
    ADD_WAIT();
    ADD_LINEAR(0, ko2);

    NEXT_MOJI();

    ADD_LINEAR(0, sa0);
    ADD_WAIT();
    ADD_LINEAR(90, sa1);
    ADD_WAIT();
    ADD_LINEAR(105, sa2);

    NEXT_MOJI();

    ADD_LINEAR(30, si0);
    ADD_WAIT();
    ADD_LINEAR(30, si1);
    ADD_WAIT();
    ADD_LINEAR(320, si2);

    NEXT_MOJI();

    ADD_LINEAR(0, su0);
    ADD_LINEAR(125, su1);
    ADD_WAIT();
    ADD_LINEAR(50, su2);

    NEXT_MOJI();

    ADD_LINEAR(345, se0);
    ADD_LINEAR(130, se1);
    ADD_WAIT();
    ADD_LINEAR(90, se2);
    ADD_LINEAR(90, se3);

    NEXT_MOJI();

    ADD_LINEAR(50, so0);
    ADD_WAIT();
    ADD_LINEAR(125, so1);

    NEXT_MOJI();

    ADD_LINEAR(130, ta0);
    ADD_WAIT();
    ADD_LINEAR(0, ta1);
    ADD_LINEAR(130, ta2);
    ADD_WAIT();
    ADD_LINEAR(50, ta3);

    NEXT_MOJI();

    ADD_LINEAR(160, ti0);
    ADD_WAIT();
    ADD_LINEAR(0, ti1);
    ADD_WAIT();
    ADD_LINEAR(110, ti2);

    NEXT_MOJI();

    ADD_LINEAR(60, tu0);
    ADD_WAIT();
    ADD_LINEAR(60, tu1);
    ADD_WAIT();
    ADD_LINEAR(120, tu2);

    NEXT_MOJI();

    ADD_LINEAR(0, te0);
    ADD_WAIT();
    ADD_LINEAR(0, te1);
    ADD_WAIT();
    ADD_LINEAR(120, te2);

    NEXT_MOJI();

    ADD_LINEAR(90, to0);
    ADD_WAIT();
    ADD_LINEAR(30, to1);

    NEXT_MOJI();

    ADD_LINEAR(0, na0);
    ADD_WAIT();
    ADD_LINEAR(100, na1);

    NEXT_MOJI();

    ADD_LINEAR(0, ni0);
    ADD_WAIT();
    ADD_LINEAR(0, ni1);

    NEXT_MOJI();

    ADD_LINEAR(0, nu0);
    ADD_LINEAR(120, nu1);
    ADD_WAIT();
    ADD_LINEAR(40, nu2);

    NEXT_MOJI();

    ADD_LINEAR(40, ne0);
    ADD_WAIT();
    ADD_LINEAR(350, ne1);
    ADD_LINEAR(140, ne2);
    ADD_WAIT();
    ADD_LINEAR(90, ne3);
    ADD_WAIT();
    ADD_LINEAR(30, ne4);

    NEXT_MOJI();

    ADD_LINEAR(125, no0);

    NEXT_MOJI();

    ADD_LINEAR(130, ha0);
    ADD_WAIT();
    ADD_LINEAR(50, ha1);

    NEXT_MOJI();

    ADD_LINEAR(350, hi0);
    ADD_WAIT();
    ADD_LINEAR(90, hi1);
    ADD_LINEAR(0, hi2);

    NEXT_MOJI();

    ADD_LINEAR(0, hu0);
    ADD_LINEAR(130, hu1);

    NEXT_MOJI();

    ADD_LINEAR(310, he0);
    ADD_LINEAR(40, he1);

    NEXT_MOJI();

    ADD_LINEAR(0, ho0);
    ADD_WAIT();
    ADD_LINEAR(90, ho1);
    ADD_LINEAR(210, ho2);
    ADD_WAIT();
    ADD_LINEAR(120, ho3);
    ADD_WAIT();
    ADD_LINEAR(50, ho4);

    NEXT_MOJI();

    ADD_LINEAR(350, ma0);
    ADD_LINEAR(130, ma1);
    ADD_WAIT();
    ADD_LINEAR(40, ma2);

    NEXT_MOJI();

    ADD_LINEAR(30, mi0);
    ADD_WAIT();
    ADD_LINEAR(30, mi1);
    ADD_WAIT();
    ADD_LINEAR(30, mi2);

    NEXT_MOJI();

    ADD_LINEAR(120, mu0);
    ADD_LINEAR(350, mu1);
    ADD_WAIT();
    ADD_LINEAR(50, mu2);

    NEXT_MOJI();

    ADD_LINEAR(125, me0);
    ADD_WAIT();
    ADD_LINEAR(40, me1);

    NEXT_MOJI();

    ADD_LINEAR(350, mo0);
    ADD_WAIT();
    ADD_LINEAR(350, mo1);
    ADD_WAIT();
    ADD_LINEAR(90, mo2);
    ADD_LINEAR(0, mo3);

    NEXT_MOJI();

    ADD_LINEAR(340, ya0);
    ADD_LINEAR(130, ya1);
    ADD_WAIT();
    ADD_LINEAR(75, ya2);

    NEXT_MOJI();

    ADD_LINEAR(350, yu0);
    ADD_LINEAR(100, yu1);
    ADD_WAIT();
    ADD_LINEAR(0, yu2);

    NEXT_MOJI();

    ADD_LINEAR(0, yo0);
    ADD_LINEAR(90, yo1);
    ADD_WAIT();
    ADD_LINEAR(0, yo2);
    ADD_WAIT();
    ADD_LINEAR(0, yo3);

    NEXT_MOJI();

    ADD_LINEAR(0, ra0);
    ADD_WAIT();
    ADD_LINEAR(0, ra1);
    ADD_LINEAR(125, ra2);

    NEXT_MOJI();

    ADD_LINEAR(90, ri0);
    ADD_WAIT();
    ADD_LINEAR(100, ri1);

    NEXT_MOJI();

    ADD_LINEAR(110, ru0);
    ADD_WAIT();
    ADD_LINEAR(90, ru1);
    ADD_LINEAR(310, ru2);

    NEXT_MOJI();

    ADD_LINEAR(90, re0);
    ADD_LINEAR(320, re1);

    NEXT_MOJI();

    ADD_LINEAR(80, ro0);
    ADD_WAIT();
    ADD_LINEAR(0, ro1);
    ADD_LINEAR(100, ro2);
    ADD_WAIT();
    ADD_LINEAR(0, ro3);

    NEXT_MOJI();

    ADD_LINEAR(90, wa0);
    ADD_WAIT();
    ADD_LINEAR(0, wa1);
    ADD_LINEAR(120, wa2);

    NEXT_MOJI();

    ADD_LINEAR(10, wo0);
    ADD_WAIT();
    ADD_LINEAR(10, wo1);
    ADD_WAIT();
    ADD_LINEAR(90, wo2);

    NEXT_MOJI();

    ADD_LINEAR(40, n0);
    ADD_WAIT();
    ADD_LINEAR(320, n1);

    NEXT_MOJI();
}
