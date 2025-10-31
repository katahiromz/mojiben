#include <windows.h>

#include <vector>
#include <map>

#include "kakijun.h"

KAKIJUN g_print_lowercase_kakijun;

#define ADD_LINEAR(angle, data) do { \
    ga.type     = STROKE::LINEAR; \
    ga.angle0   = angle; \
    ga.res      = (100 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_POLAR(a0, a1, center_x, center_y, data) do { \
    ga.type     = STROKE::POLAR; \
    ga.angle0   = a0; \
    ga.angle1   = a1; \
    ga.cx       = (center_x * 254) / 300; \
    ga.cy       = (center_y * 254) / 300; \
    ga.res      = (100 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_WAIT() do { \
    ga.type     = STROKE::WAIT; \
    vga.push_back(ga); \
} while (0)

#define ADD_DOT(data) do { \
    ga.type     = STROKE::DOT; \
    ga.res      = (100 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define NEXT_MOJI() do { \
    g_print_lowercase_kakijun[index++] = vga; \
    vga.clear(); \
    ++j; i = 0; \
} while (0)

VOID InitPrintLowerCase(VOID)
{
    STROKE ga;
    std::vector<STROKE> vga;
    size_t index = 0;
    INT j = 0, i = 0;

    ADD_POLAR(360, 0, 150, 150, a0);
    ADD_WAIT();
    ADD_LINEAR(90, a1);

    NEXT_MOJI();

    ADD_LINEAR(90, b0);
    ADD_WAIT();
    ADD_POLAR(180, 540, 150, 150, b1);

    NEXT_MOJI();

    ADD_POLAR(355, 15, 150, 151, c0);

    NEXT_MOJI();

    ADD_POLAR(360, 0, 147, 151, d0);
    ADD_WAIT();
    ADD_LINEAR(90, d1);

    NEXT_MOJI();

    ADD_LINEAR(0, e0);
    ADD_WAIT();
    ADD_POLAR(375, 20, 147, 150, e1);

    NEXT_MOJI();

    ADD_POLAR(385, 180, 164, 42, f0);
    ADD_LINEAR(90, f1);
    ADD_WAIT();
    ADD_LINEAR(0, f2);

    NEXT_MOJI();

    ADD_POLAR(360, 0, 147, 149, g0);
    ADD_WAIT();
    ADD_LINEAR(90, g1);
    ADD_POLAR(0, 190, 148, 241, g2);

    NEXT_MOJI();

    ADD_LINEAR(90, h0);
    ADD_WAIT();
    ADD_POLAR(180, 360, 149, 137, h1);
    ADD_LINEAR(90, h2);

    NEXT_MOJI();

    ADD_LINEAR(90, i0);
    ADD_WAIT();
    ADD_DOT(i1);

    NEXT_MOJI();

    ADD_LINEAR(90, j0);
    ADD_POLAR(0, 210, 148, 264, j1);
    ADD_WAIT();
    ADD_DOT(j2);

    NEXT_MOJI();

    ADD_LINEAR(90, k0);
    ADD_WAIT();
    ADD_LINEAR(140, k1);
    ADD_WAIT();
    ADD_LINEAR(50, k2);

    NEXT_MOJI();

    ADD_LINEAR(90, l0);

    NEXT_MOJI();

    ADD_LINEAR(90, m0);
    ADD_WAIT();
    ADD_POLAR(180, 360, 115, 138, m1);
    ADD_LINEAR(90, m2);
    ADD_WAIT();
    ADD_POLAR(180, 360, 181, 138, m3);
    ADD_LINEAR(90, m4);

    NEXT_MOJI();

    ADD_LINEAR(90, n0);
    ADD_WAIT();
    ADD_POLAR(180, 365, 148, 137, n1);
    ADD_LINEAR(90, n2);

    NEXT_MOJI();

    ADD_POLAR(270, -90, 148, 151, o0);

    NEXT_MOJI();

    ADD_LINEAR(90, p0);
    ADD_WAIT();
    ADD_POLAR(180, 540, 148, 151, p1);

    NEXT_MOJI();

    ADD_POLAR(360, 0, 148, 151, q0);
    ADD_WAIT();
    ADD_LINEAR(90, q1);

    NEXT_MOJI();

    ADD_LINEAR(90, r0);
    ADD_WAIT();
    ADD_POLAR(215, 340, 149, 145, r1);

    NEXT_MOJI();

    ADD_POLAR(375, 90, 153, 128, s0);
    ADD_POLAR(270, 540, 146, 162, s1);

    NEXT_MOJI();

    ADD_LINEAR(90, t0);
    ADD_WAIT();
    ADD_LINEAR(0, t1);

    NEXT_MOJI();

    ADD_LINEAR(90, u0);
    ADD_POLAR(175, -10, 151, 164, u1);
    ADD_WAIT();
    ADD_LINEAR(90, u2);

    NEXT_MOJI();

    ADD_LINEAR(70, v0);
    ADD_WAIT();
    ADD_LINEAR(110, v1);

    NEXT_MOJI();

    ADD_LINEAR(70, w0);
    ADD_WAIT();
    ADD_LINEAR(110, w1);
    ADD_WAIT();
    ADD_LINEAR(70, w2);
    ADD_WAIT();
    ADD_LINEAR(110, w3);

    NEXT_MOJI();

    ADD_LINEAR(55, x0);
    ADD_WAIT();
    ADD_LINEAR(125, x1);

    NEXT_MOJI();

    ADD_LINEAR(65, y0);
    ADD_WAIT();
    ADD_LINEAR(110, y1);

    NEXT_MOJI();

    ADD_LINEAR(0, z0);
    ADD_WAIT();
    ADD_LINEAR(130, z1);
    ADD_WAIT();
    ADD_LINEAR(0, z2);

    NEXT_MOJI();
}
