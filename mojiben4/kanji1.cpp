// Moji No Benkyo (3)
// Copyright (C) 2019-2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include <windows.h>

#include <vector>
#include <map>
#include <cassert>

#include "kakijun.h"

// Kakijun region data
KAKIJUN g_kanji1_kakijun;
INT g_map[80];

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
    g_kanji1_kakijun[index++] = vga; \
    vga.clear(); \
    ++j; i = 0; \
} while (0)

VOID InitKanji1(VOID)
{
    STROKE ga;
    std::vector<STROKE> vga;
    size_t index = 0;
    INT j = 0, i = 0;

    ADD_LINEAR(0, kanji000_00);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji001_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji001_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji001_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji001_03);
    ADD_LINEAR(90, kanji001_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji001_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji002_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji002_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji002_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji002_03);
    ADD_LINEAR(90, kanji002_04);
    ADD_LINEAR(-180, kanji002_05);
    ADD_WAIT();
    ADD_LINEAR(45, kanji002_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji002_07);
    ADD_WAIT();
    ADD_LINEAR(45, kanji002_08);
    ADD_WAIT();
    ADD_LINEAR(45, kanji002_09);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji003_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji003_01);
    ADD_LINEAR(90, kanji003_02);
    ADD_LINEAR(180, kanji003_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji003_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji003_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji004_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji004_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji004_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji004_03);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji005_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji005_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji005_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji005_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji005_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji005_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji005_06);
    ADD_LINEAR(90, kanji005_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji005_08);
    ADD_WAIT();
    ADD_LINEAR(0, kanji005_09);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji006_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji006_01);
    ADD_WAIT();
    ADD_LINEAR(45, kanji006_02);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji007_00);
    ADD_WAIT();
    ADD_LINEAR(135, kanji007_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji007_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji007_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji008_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji008_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji008_02);
    ADD_WAIT();
    ADD_LINEAR(135, kanji008_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji008_04);
    ADD_WAIT();
    ADD_LINEAR(180, kanji008_05);
    ADD_WAIT();
    ADD_LINEAR(90, kanji008_06);
    ADD_LINEAR(0, kanji008_07);
    ADD_LINEAR(270, kanji008_08);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji009_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji009_01);
    ADD_LINEAR(90, kanji009_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji009_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji009_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji009_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji009_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji009_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji010_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji010_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji010_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji010_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji010_04);
    ADD_LINEAR(135, kanji010_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji010_06);
    ADD_LINEAR(135, kanji010_07);
    ADD_WAIT();
    ADD_LINEAR(90, kanji010_08);
    ADD_LINEAR(135, kanji010_09);
    ADD_WAIT();
    ADD_LINEAR(0, kanji010_10);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji011_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji011_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji011_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji011_03);
    ADD_LINEAR(90, kanji011_04);
    ADD_LINEAR(270, kanji011_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji011_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji011_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji012_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji012_01);
    ADD_LINEAR(90, kanji012_02);
    ADD_LINEAR(0, kanji012_03);
    ADD_LINEAR(270, kanji012_04);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji013_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji013_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji013_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji013_03);
    ADD_WAIT();
    ADD_LINEAR(135, kanji013_04);
    ADD_WAIT();
    ADD_LINEAR(45, kanji013_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji014_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji014_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji014_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji014_03);
    ADD_WAIT();
    ADD_LINEAR(45, kanji014_04);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji015_00);
    ADD_WAIT();
    ADD_LINEAR(45, kanji015_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji015_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji015_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji015_04);
    ADD_WAIT();
    ADD_LINEAR(45, kanji015_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji015_06);
    ADD_WAIT();
    ADD_LINEAR(0, kanji015_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji016_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji016_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji016_02);
    ADD_LINEAR(135, kanji016_03);
    ADD_WAIT();
    ADD_LINEAR(135, kanji016_04);
    ADD_WAIT();
    ADD_LINEAR(45, kanji016_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji016_06);
    ADD_WAIT();
    ADD_LINEAR(90, kanji016_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji016_08);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji017_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji017_01);
    ADD_LINEAR(90, kanji017_02);
    ADD_LINEAR(135, kanji017_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji017_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji017_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji018_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji018_01);
    ADD_WAIT();
    ADD_LINEAR(45, kanji018_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji018_03);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji019_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji019_01);
    ADD_LINEAR(90, kanji019_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji019_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji019_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji019_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji019_06);
    ADD_WAIT();
    ADD_LINEAR(90, kanji019_07);
    ADD_LINEAR(0, kanji019_08);
    ADD_LINEAR(270, kanji019_09);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji020_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji020_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji020_02);
    ADD_LINEAR(90, kanji020_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji020_04);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji021_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji021_01);
    ADD_LINEAR(90, kanji021_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji021_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji022_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji022_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji022_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji022_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji022_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji022_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji022_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji022_07);
    ADD_WAIT();
    ADD_LINEAR(135, kanji022_08);
    ADD_WAIT();
    ADD_LINEAR(45, kanji022_09);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji023_00);
    ADD_WAIT();
    ADD_LINEAR(135, kanji023_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji023_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji023_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji023_04);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji024_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji024_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji024_02);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji025_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji025_01);
    ADD_LINEAR(0, kanji025_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji025_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji026_00);
    ADD_LINEAR(135, kanji026_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji026_02);
    ADD_LINEAR(225, kanji026_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji026_04);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji027_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji027_01);
    ADD_LINEAR(90, kanji027_02);
    ADD_WAIT();
    ADD_LINEAR(135, kanji027_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji027_04);
    ADD_LINEAR(0, kanji027_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji027_06);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji028_00);
    ADD_LINEAR(45, kanji028_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji028_02);
    ADD_LINEAR(0, kanji028_03);
    ADD_WAIT();
    ADD_LINEAR(45, kanji028_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji028_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji028_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji028_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji029_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji029_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji029_02);
    ADD_LINEAR(135, kanji029_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji029_04);
    ADD_LINEAR(135, kanji029_05);
    ADD_WAIT();
    ADD_LINEAR(90, kanji029_06);
    ADD_LINEAR(180, kanji029_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji029_08);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji030_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji030_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji030_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji030_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji030_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji030_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji031_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji031_01);
    ADD_LINEAR(0, kanji031_02);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji032_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji032_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji032_02);
    ADD_LINEAR(90, kanji032_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji032_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji032_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji032_06);
    ADD_WAIT();
    ADD_LINEAR(90, kanji032_07);

    NEXT_MOJI();

    ADD_LINEAR(180, kanji033_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji033_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji033_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji033_03);
    ADD_LINEAR(180, kanji033_04);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji034_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji034_01);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji035_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji035_01);
    ADD_LINEAR(0, kanji035_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji035_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji035_04);
    ADD_LINEAR(0, kanji035_05);
    ADD_WAIT();
    ADD_LINEAR(90, kanji035_06);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji036_00);
    ADD_LINEAR(45, kanji036_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji036_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji036_03);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji037_00);
    ADD_LINEAR(180, kanji037_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji037_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji037_03);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji038_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji038_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji038_02);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji039_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji039_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji039_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji039_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji039_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji039_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji039_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji039_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji039_08);
    ADD_WAIT();
    ADD_LINEAR(90, kanji039_09);
    ADD_WAIT();
    ADD_LINEAR(135, kanji039_10);
    ADD_WAIT();
    ADD_LINEAR(45, kanji039_11);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji040_00);
    ADD_WAIT();
    ADD_LINEAR(45, kanji040_01);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji041_00);
    ADD_LINEAR(180, kanji041_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji041_02);
    ADD_LINEAR(135, kanji041_03);
    ADD_WAIT();
    ADD_LINEAR(180, kanji041_04);
    ADD_WAIT();
    ADD_LINEAR(45, kanji041_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji042_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji042_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji042_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji042_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji042_04);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji043_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji043_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji043_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji043_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji043_04);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji044_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji044_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji044_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji044_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji044_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji044_05);
    ADD_LINEAR(90, kanji044_06);
    ADD_LINEAR(180, kanji044_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji044_08);
    ADD_WAIT();
    ADD_LINEAR(0, kanji044_09);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji045_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji045_01);
    ADD_LINEAR(135, kanji045_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji045_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji046_00);
    ADD_WAIT();
    ADD_LINEAR(135, kanji046_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji046_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji046_03);
    ADD_LINEAR(90, kanji046_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji046_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji047_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji047_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji047_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji047_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji047_04);
    ADD_LINEAR(180, kanji047_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji047_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji047_07);

    NEXT_MOJI();

    ADD_LINEAR(180, kanji048_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji048_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji048_02);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji049_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji049_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji049_02);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji050_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji050_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji050_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji050_03);
    ADD_WAIT();
    ADD_LINEAR(135, kanji050_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji050_05);
    ADD_LINEAR(0, kanji050_06);
    ADD_LINEAR(270, kanji050_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji051_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji051_01);
    ADD_LINEAR(90, kanji051_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji051_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji051_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji051_05);
    ADD_WAIT();
    ADD_LINEAR(90, kanji051_06);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji052_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji052_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji052_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji052_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji052_04);
    ADD_LINEAR(90, kanji052_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji052_06);
    ADD_WAIT();
    ADD_LINEAR(0, kanji052_07);
    ADD_WAIT();
    ADD_LINEAR(0, kanji052_08);
    ADD_WAIT();
    ADD_LINEAR(90, kanji052_09);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji053_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji053_01);
    ADD_LINEAR(90, kanji053_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji053_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji053_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji053_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji053_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji053_07);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji054_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji054_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji054_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji054_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji054_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji054_05);
    ADD_LINEAR(180, kanji054_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji054_07);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji055_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji055_01);
    ADD_WAIT();
    ADD_LINEAR(45, kanji055_02);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji056_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji056_01);
    ADD_LINEAR(90, kanji056_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji056_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji056_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji056_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji056_06);
    ADD_LINEAR(90, kanji056_07);
    ADD_LINEAR(180, kanji056_08);
    ADD_WAIT();
    ADD_LINEAR(135, kanji056_09);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji057_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji057_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji057_02);
    ADD_WAIT();
    ADD_LINEAR(135, kanji057_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji057_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji057_05);
    ADD_LINEAR(180, kanji057_06);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji058_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji058_01);
    ADD_LINEAR(90, kanji058_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji058_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji058_04);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji059_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji059_01);
    ADD_LINEAR(90, kanji059_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji059_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji059_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji059_05);
    ADD_WAIT();
    ADD_LINEAR(45, kanji059_06);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji060_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji060_01);
    ADD_LINEAR(90, kanji060_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji060_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji060_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji060_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji060_06);
    ADD_WAIT();
    ADD_LINEAR(90, kanji060_07);
    ADD_LINEAR(180, kanji060_08);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji061_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji061_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji061_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji061_03);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji062_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji062_01);
    ADD_LINEAR(90, kanji062_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji062_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji062_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji062_05);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji063_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji063_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji063_02);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji064_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji064_01);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji065_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji065_01);
    ADD_LINEAR(90, kanji065_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji065_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji065_04);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji066_00);
    ADD_WAIT();
    ADD_LINEAR(45, kanji066_01);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji067_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji067_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji067_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji067_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji067_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji067_05);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji068_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji068_01);
    ADD_WAIT();
    ADD_LINEAR(0, kanji068_02);
    ADD_LINEAR(90, kanji068_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji068_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji068_05);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji069_00);
    ADD_WAIT();
    ADD_LINEAR(45, kanji069_01);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji070_00);
    ADD_WAIT();
    ADD_LINEAR(135, kanji070_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji070_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji070_03);
    ADD_LINEAR(90, kanji070_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji070_05);
    ADD_WAIT();
    ADD_LINEAR(0, kanji070_06);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji071_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji071_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji071_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji071_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji072_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji072_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji072_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji072_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji073_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji073_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji073_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji073_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji073_04);

    NEXT_MOJI();

    ADD_LINEAR(135, kanji074_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji074_01);
    ADD_LINEAR(135, kanji074_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji074_03);
    ADD_WAIT();
    ADD_LINEAR(90, kanji074_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji074_05);
    ADD_LINEAR(90, kanji074_06);
    ADD_WAIT();
    ADD_LINEAR(0, kanji074_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji075_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji075_01);
    ADD_LINEAR(90, kanji075_02);
    ADD_WAIT();
    ADD_LINEAR(0, kanji075_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji075_04);
    ADD_WAIT();
    ADD_LINEAR(0, kanji075_05);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji076_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji076_01);
    ADD_WAIT();
    ADD_LINEAR(90, kanji076_02);
    ADD_WAIT();
    ADD_LINEAR(90, kanji076_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji076_04);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji077_00);
    ADD_LINEAR(90, kanji077_01);
    ADD_LINEAR(180, kanji077_02);
    ADD_WAIT();
    ADD_LINEAR(135, kanji077_03);

    NEXT_MOJI();

    ADD_LINEAR(0, kanji078_00);
    ADD_WAIT();
    ADD_LINEAR(90, kanji078_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji078_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji078_03);
    ADD_WAIT();
    ADD_LINEAR(0, kanji078_04);
    ADD_WAIT();
    ADD_LINEAR(90, kanji078_05);
    ADD_WAIT();
    ADD_LINEAR(135, kanji078_06);
    ADD_WAIT();
    ADD_LINEAR(45, kanji078_07);

    NEXT_MOJI();

    ADD_LINEAR(90, kanji079_00);
    ADD_WAIT();
    ADD_LINEAR(0, kanji079_01);
    ADD_WAIT();
    ADD_LINEAR(135, kanji079_02);
    ADD_WAIT();
    ADD_LINEAR(45, kanji079_03);

    NEXT_MOJI();

    assert(index == 80);

    index = 0;
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
