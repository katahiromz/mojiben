// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// stroke type
enum TYPE
{
    WAIT,
    LINEAR,
    POLAR,
    DOT
};

// stroke info
struct STROKE
{
    TYPE type;
    INT angle0;
    INT angle1;
    INT cx;
    INT cy;
    INT res;
};
typedef std::vector<STROKE> KAKIJUN[160];

extern KAKIJUN g_kanji2_kakijun;

VOID InitKanji2(VOID);

// 書き順イメージのサイズ。
#define KAKIJUN_SIZE 254
// 書き順イメージの中心点。
#define KAKIJUN_CENTER_XY (KAKIJUN_SIZE / 2)
