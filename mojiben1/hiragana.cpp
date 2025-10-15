// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>

#include <vector>
#include <map>

#include "kakijun.h"

// Kakijun region data
KAKIJUN g_hiragana_kakijun;

#define ADD_LINEAR(angle, data) do { \
    ga.type     = LINEAR; \
    ga.angle0   = angle; \
    ga.res      = (100 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_POLAR(a0, a1, center_x, center_y, data) do { \
    ga.type     = POLAR; \
    ga.angle0   = a0; \
    ga.angle1   = a1; \
    ga.cx       = (center_x * 254) / 300; \
    ga.cy       = (center_y * 254) / 300; \
    ga.res      = (100 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_WAIT() do { \
    ga.type     = WAIT; \
    vga.push_back(ga); \
} while (0)

#define NEXT_MOJI() do { \
    g_hiragana_kakijun.push_back(vga); \
    vga.clear(); \
    ++j; i = 0; \
} while (0)

VOID InitHiragana(VOID)
{
    STROKE ga;
    std::vector<STROKE> vga;
    size_t index = 0;
    INT j = 0, i = 0;

    ADD_LINEAR(350, a0);
    ADD_WAIT();
    ADD_LINEAR(90, a1);
    ADD_WAIT();
    ADD_LINEAR(125, a2);
    ADD_POLAR(140, 450, 140, 195, a3);

    NEXT_MOJI();

    ADD_LINEAR(70, i0);
    ADD_LINEAR(295, i1);
    ADD_WAIT();
    ADD_LINEAR(55, i2);

    NEXT_MOJI();

    ADD_LINEAR(10, u0);
    ADD_WAIT();
    ADD_LINEAR(340, u1);
    ADD_LINEAR(95, u2);

    NEXT_MOJI();

    ADD_LINEAR(10, e0);
    ADD_WAIT();
    ADD_LINEAR(340, e1);
    ADD_LINEAR(130, e2);
    ADD_LINEAR(320, e3);
    ADD_LINEAR(90, e4);
    ADD_LINEAR(5, e5);

    NEXT_MOJI();

    ADD_LINEAR(340, o0);
    ADD_WAIT();
    ADD_LINEAR(90, o1);
    ADD_POLAR(80, 275, 102, 219, o2);
    ADD_LINEAR(340, o3);
    ADD_POLAR(270, 480, 199, 190, o4);
    ADD_WAIT();
    ADD_LINEAR(40, o5);

    NEXT_MOJI();

    ADD_LINEAR(345, ka0);
    ADD_LINEAR(110, ka1);
    ADD_LINEAR(220, ka2);
    ADD_WAIT();
    ADD_LINEAR(110, ka3);
    ADD_WAIT();
    ADD_LINEAR(55, ka4);

    NEXT_MOJI();

    ADD_LINEAR(345, ki0);
    ADD_WAIT();
    ADD_LINEAR(15, ki1);
    ADD_WAIT();
    ADD_LINEAR(60, ki2);
    ADD_LINEAR(190, ki3);
    ADD_WAIT();
    ADD_LINEAR(30, ki4);

    NEXT_MOJI();

    ADD_LINEAR(90, ku0);

    NEXT_MOJI();

    ADD_LINEAR(90, ke0);
    ADD_LINEAR(290, ke1);
    ADD_WAIT();
    ADD_LINEAR(350, ke2);
    ADD_WAIT();
    ADD_LINEAR(105, ke3);

    NEXT_MOJI();

    ADD_LINEAR(5, ko0);
    ADD_LINEAR(160, ko1);
    ADD_WAIT();
    ADD_LINEAR(20, ko2);

    NEXT_MOJI();

    ADD_LINEAR(15, sa0);
    ADD_WAIT();
    ADD_LINEAR(55, sa1);
    ADD_LINEAR(190, sa2);
    ADD_WAIT();
    ADD_LINEAR(30, sa3);

    NEXT_MOJI();

    ADD_LINEAR(90, si0);
    ADD_LINEAR(0, si1);

    NEXT_MOJI();

    ADD_LINEAR(355, su0);
    ADD_WAIT();
    ADD_LINEAR(90, su1);
    ADD_POLAR(90, 360, 148, 160, su2);
    ADD_LINEAR(110, su3);

    NEXT_MOJI();

    ADD_LINEAR(350, se0);
    ADD_WAIT();
    ADD_LINEAR(100, se1);
    ADD_LINEAR(220, se2);
    ADD_WAIT();
    ADD_LINEAR(90, se3);
    ADD_LINEAR(0, se4);

    NEXT_MOJI();

    ADD_LINEAR(350, so0);
    ADD_LINEAR(145, so1);
    ADD_LINEAR(350, so2);
    ADD_POLAR(305, 85, 212, 187, so3);

    NEXT_MOJI();

    ADD_LINEAR(350, ta0);
    ADD_WAIT();
    ADD_LINEAR(105, ta1);
    ADD_WAIT();
    ADD_LINEAR(0, ta2);
    ADD_WAIT();
    ADD_LINEAR(20, ta3);

    NEXT_MOJI();

    ADD_LINEAR(350, ti0);
    ADD_WAIT();
    ADD_LINEAR(100, ti1);
    ADD_LINEAR(335, ti2);
    ADD_POLAR(270, 500, 189, 196, ti3);

    NEXT_MOJI();

    ADD_LINEAR(15, tu0);
    ADD_POLAR(280, 490, 175, 150, tu1);

    NEXT_MOJI();

    ADD_LINEAR(350, te0);
    ADD_LINEAR(135, te1);
    ADD_LINEAR(50, te2);

    NEXT_MOJI();

    ADD_LINEAR(55, to0);
    ADD_WAIT();
    ADD_LINEAR(140, to1);
    ADD_LINEAR(20, to2);

    NEXT_MOJI();

    ADD_LINEAR(345, na0);
    ADD_WAIT();
    ADD_LINEAR(115, na1);
    ADD_WAIT();
    ADD_LINEAR(20, na2);
    ADD_WAIT();
    ADD_LINEAR(90, na3);
    ADD_POLAR(20, 270, 144, 226, na4);
    ADD_LINEAR(20, na5);

    NEXT_MOJI();

    ADD_LINEAR(90, ni0);
    ADD_LINEAR(290, ni1);
    ADD_WAIT();
    ADD_LINEAR(0, ni2);
    ADD_WAIT();
    ADD_LINEAR(20, ni3);

    NEXT_MOJI();

    ADD_LINEAR(75, nu0);
    ADD_WAIT();
    ADD_LINEAR(115, nu1);
    ADD_POLAR(135, 425, 140, 164, nu2);
    ADD_POLAR(90, 270, 192, 208, nu3);
    ADD_LINEAR(30, nu4);

    NEXT_MOJI();

    ADD_LINEAR(90, ne0);
    ADD_WAIT();
    ADD_LINEAR(340, ne1);
    ADD_LINEAR(120, ne2);
    ADD_LINEAR(315, ne3);
    ADD_LINEAR(90, ne4);
    ADD_POLAR(40, 290, 189, 219, ne5);
    ADD_LINEAR(30, ne6);

    NEXT_MOJI();

    ADD_LINEAR(115, no0);
    ADD_POLAR(135, 450, 143, 152, no1);

    NEXT_MOJI();

    ADD_LINEAR(90, ha0);
    ADD_LINEAR(290, ha1);
    ADD_WAIT();
    ADD_LINEAR(350, ha2);
    ADD_WAIT();
    ADD_LINEAR(90, ha3);
    ADD_POLAR(30, 280, 175, 222, ha4);
    ADD_LINEAR(20, ha5);

    NEXT_MOJI();

    ADD_LINEAR(340, hi0);
    ADD_LINEAR(120, hi1);
    ADD_POLAR(210, 10, 119, 167, hi2);
    ADD_LINEAR(285, hi3);
    ADD_LINEAR(60, hi4);

    NEXT_MOJI();

    ADD_LINEAR(30, hu0);
    ADD_LINEAR(160, hu1);
    ADD_WAIT();
    ADD_POLAR(280, 455, 100, 181, hu2);
    ADD_WAIT();
    ADD_LINEAR(60, hu3);
    ADD_LINEAR(300, hu4);
    ADD_WAIT();
    ADD_LINEAR(55, hu5);

    NEXT_MOJI();

    ADD_LINEAR(315, he0);
    ADD_LINEAR(40, he1);

    NEXT_MOJI();

    ADD_LINEAR(90, ho0);
    ADD_LINEAR(295, ho1);
    ADD_WAIT();
    ADD_LINEAR(355, ho2);
    ADD_WAIT();
    ADD_LINEAR(350, ho3);
    ADD_WAIT();
    ADD_LINEAR(90, ho4);
    ADD_POLAR(60, 270, 188, 224, ho5);
    ADD_LINEAR(30, ho6);

    NEXT_MOJI();

    ADD_LINEAR(350, ma0);
    ADD_WAIT();
    ADD_LINEAR(10, ma1);
    ADD_WAIT();
    ADD_LINEAR(90, ma2);
    ADD_POLAR(40, 280, 123, 228, ma3);
    ADD_LINEAR(20, ma4);

    NEXT_MOJI();

    ADD_LINEAR(350, mi0);
    ADD_LINEAR(120, mi1);
    ADD_POLAR(100, 285, 66, 200, mi2);
    ADD_LINEAR(10, mi3);
    ADD_WAIT();
    ADD_LINEAR(120, mi4);

    NEXT_MOJI();

    ADD_LINEAR(15, mu0);
    ADD_WAIT();
    ADD_LINEAR(95, mu1);
    ADD_POLAR(80, 350, 70, 180, mu2);
    ADD_POLAR(175, 0, 153, 156, mu3);
    ADD_WAIT();
    ADD_LINEAR(35, mu4);

    NEXT_MOJI();

    ADD_LINEAR(75, me0);
    ADD_WAIT();
    ADD_LINEAR(120, me1);
    ADD_POLAR(135, 440, 134, 172, me2);

    NEXT_MOJI();

    ADD_LINEAR(95, mo0);
    ADD_POLAR(170, -60, 152, 198, mo1);
    ADD_WAIT();
    ADD_LINEAR(0, mo2);
    ADD_WAIT();
    ADD_LINEAR(10, mo3);

    NEXT_MOJI();

    ADD_LINEAR(345, ya0);
    ADD_POLAR(270, 480, 200, 116, ya1);
    ADD_WAIT();
    ADD_LINEAR(50, ya2);
    ADD_LINEAR(200, ya3);
    ADD_WAIT();
    ADD_LINEAR(70, ya4);

    NEXT_MOJI();

    ADD_LINEAR(85, yu0);
    ADD_POLAR(160, 340, 166, 191, yu1);
    ADD_POLAR(0, 155, 184, 127, yu2);
    ADD_WAIT();
    ADD_LINEAR(95, yu3);

    NEXT_MOJI();

    ADD_LINEAR(350, yo0);
    ADD_WAIT();
    ADD_LINEAR(90, yo1);
    ADD_POLAR(50, 270, 129, 227, yo2);
    ADD_LINEAR(20, yo3);

    NEXT_MOJI();

    ADD_LINEAR(35, ra0);
    ADD_LINEAR(160, ra1);
    ADD_WAIT();
    ADD_LINEAR(95, ra2);
    ADD_LINEAR(330, ra3);
    ADD_POLAR(270, 495, 186, 189, ra4);

    NEXT_MOJI();

    ADD_LINEAR(90, ri0);
    ADD_LINEAR(290, ri1);
    ADD_WAIT();
    ADD_LINEAR(100, ri2);

    NEXT_MOJI();

    ADD_LINEAR(350, ru0);
    ADD_LINEAR(130, ru1);
    ADD_LINEAR(340, ru2);
    ADD_POLAR(270, 495, 169, 178, ru3);
    ADD_POLAR(90, 285, 135, 235, ru4);
    ADD_LINEAR(30, ru5);

    NEXT_MOJI();

    ADD_LINEAR(95, re0);
    ADD_WAIT();
    ADD_LINEAR(340, re1);
    ADD_LINEAR(125, re2);
    ADD_LINEAR(320, re3);
    ADD_LINEAR(90, re4);
    ADD_LINEAR(340, re5);

    NEXT_MOJI();

    ADD_LINEAR(350, ro0);
    ADD_LINEAR(130, ro1);
    ADD_LINEAR(335, ro2);
    ADD_POLAR(270, 480, 168, 180, ro3);

    NEXT_MOJI();

    ADD_LINEAR(95, wa0);
    ADD_WAIT();
    ADD_LINEAR(340, wa1);
    ADD_LINEAR(120, wa2);
    ADD_LINEAR(330, wa3);
    ADD_POLAR(270, 480, 188, 163, wa4);

    NEXT_MOJI();

    ADD_LINEAR(355, wo0);
    ADD_WAIT();
    ADD_LINEAR(120, wo1);
    ADD_POLAR(225, 380, 96, 197, wo2);
    ADD_WAIT();
    ADD_LINEAR(150, wo3);
    ADD_LINEAR(10, wo4);

    NEXT_MOJI();

    ADD_LINEAR(120, n0);
    ADD_LINEAR(310, n1);
    ADD_POLAR(195, 5, 200, 155, n2);

    NEXT_MOJI();
}
