#include <windows.h>

#include <vector>
#include <map>

#include "kakijun.h"

KAKIJUN g_print_uppercase_kakijun;

#define ADD_LINEAR(angle, data) do { \
    ga.type     = STROKE::LINEAR; \
    ga.angle0   = angle; \
    ga.res      = (200 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_POLAR(a0, a1, center_x, center_y, data) do { \
    ga.type     = STROKE::POLAR; \
    ga.angle0   = a0; \
    ga.angle1   = a1; \
    ga.cx       = (center_x * 254) / 300; \
    ga.cy       = (center_y * 254) / 300; \
    ga.res      = (200 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define ADD_WAIT() do { \
    ga.type     = STROKE::WAIT; \
    vga.push_back(ga); \
} while (0)

#define ADD_DOT(data) do { \
    ga.type     = STROKE::DOT; \
    ga.res      = (200 + j) * 100 + i; \
    vga.push_back(ga); \
    ++i; \
} while (0)

#define NEXT_MOJI() do { \
    g_print_uppercase_kakijun[index++] = vga; \
    vga.clear(); \
    ++j; i = 0; \
} while (0)

VOID InitPrintUpperCase(VOID)
{
    STROKE ga;
    std::vector<STROKE> vga;
    size_t index = 0;
    INT j = 0, i = 0;

    ADD_LINEAR(115, A0);
    ADD_WAIT();
    ADD_LINEAR(65, A1);
    ADD_WAIT();
    ADD_LINEAR(0, A2);

    NEXT_MOJI();

    ADD_LINEAR(90, B0);
    ADD_WAIT();
    ADD_LINEAR(0, B1);
    ADD_POLAR(270, 455, 154, 55, B2);
    ADD_WAIT();
    ADD_LINEAR(0, B3);
    ADD_POLAR(270, 450, 173, 153, B4);
    ADD_LINEAR(180, B5);

    NEXT_MOJI();

    ADD_POLAR(340, 20, 154, 106, C0);

    NEXT_MOJI();

    ADD_LINEAR(90, D0);
    ADD_WAIT();
    ADD_LINEAR(0, D1);
    ADD_POLAR(270, 450, 133, 106, D2);
    ADD_LINEAR(180, D3);

    NEXT_MOJI();

    ADD_LINEAR(90, E0);
    ADD_WAIT();
    ADD_LINEAR(0, E1);
    ADD_WAIT();
    ADD_LINEAR(0, E2);
    ADD_WAIT();
    ADD_LINEAR(0, E3);

    NEXT_MOJI();

    ADD_LINEAR(90, F0);
    ADD_WAIT();
    ADD_LINEAR(0, F1);
    ADD_WAIT();
    ADD_LINEAR(0, F2);

    NEXT_MOJI();

    ADD_POLAR(340, 25, 155, 105, G0);
    ADD_WAIT();
    ADD_LINEAR(0, G1);
    ADD_LINEAR(90, G2);

    NEXT_MOJI();

    ADD_LINEAR(90, H0);
    ADD_WAIT();
    ADD_LINEAR(90, H1);
    ADD_WAIT();
    ADD_LINEAR(0, H2);

    NEXT_MOJI();

    ADD_LINEAR(90, I0);
    ADD_WAIT();
    ADD_LINEAR(0, I1);
    ADD_WAIT();
    ADD_LINEAR(0, I2);

    NEXT_MOJI();

    ADD_LINEAR(90, J0);
    ADD_POLAR(0, 190, 150, 141, J1);

    NEXT_MOJI();

    ADD_LINEAR(90, K0);
    ADD_WAIT();
    ADD_LINEAR(135, K1);
    ADD_WAIT();
    ADD_LINEAR(45, K2);

    NEXT_MOJI();

    ADD_LINEAR(90, L0);
    ADD_WAIT();
    ADD_LINEAR(0, L1);

    NEXT_MOJI();

    ADD_LINEAR(90, M0);
    ADD_WAIT();
    ADD_LINEAR(90, M1);
    ADD_WAIT();
    ADD_LINEAR(65, M2);
    ADD_WAIT();
    ADD_LINEAR(115, M3);

    NEXT_MOJI();

    ADD_LINEAR(90, N0);
    ADD_WAIT();
    ADD_LINEAR(90, N1);
    ADD_WAIT();
    ADD_LINEAR(50, N2);

    NEXT_MOJI();

    ADD_POLAR(270, -90, 149, 105, O0);

    NEXT_MOJI();

    ADD_LINEAR(90, P0);
    ADD_WAIT();
    ADD_POLAR(230, 485, 111, 61, P1);

    NEXT_MOJI();

    ADD_POLAR(270, -90, 149, 105, Q0);
    ADD_WAIT();
    ADD_LINEAR(45, Q1);

    NEXT_MOJI();

    ADD_LINEAR(90, R0);
    ADD_WAIT();
    ADD_POLAR(230, 485, 111, 61, R1);
    ADD_WAIT();
    ADD_LINEAR(50, R2);

    NEXT_MOJI();

    ADD_POLAR(370, 90, 152, 59, S0);
    ADD_POLAR(-90, 190, 144, 147, S1);

    NEXT_MOJI();

    ADD_LINEAR(90, T0);
    ADD_WAIT();
    ADD_LINEAR(0, T1);

    NEXT_MOJI();

    ADD_LINEAR(90, U0);
    ADD_POLAR(170, 10, 149, 140, U1);
    ADD_LINEAR(270, U2);

    NEXT_MOJI();

    ADD_LINEAR(65, V0);
    ADD_WAIT();
    ADD_LINEAR(115, V1);

    NEXT_MOJI();

    ADD_LINEAR(75, W0);
    ADD_WAIT();
    ADD_LINEAR(105, W1);
    ADD_WAIT();
    ADD_LINEAR(75, W2);
    ADD_WAIT();
    ADD_LINEAR(105, W3);

    NEXT_MOJI();

    ADD_LINEAR(55, X0);
    ADD_WAIT();
    ADD_LINEAR(125, X1);

    NEXT_MOJI();

    ADD_LINEAR(55, Y0);
    ADD_WAIT();
    ADD_LINEAR(125, Y1);
    ADD_WAIT();
    ADD_LINEAR(90, Y2);

    NEXT_MOJI();

    ADD_LINEAR(0, Z0);
    ADD_WAIT();
    ADD_LINEAR(125, Z1);
    ADD_WAIT();
    ADD_LINEAR(0, Z2);

    NEXT_MOJI();
}
