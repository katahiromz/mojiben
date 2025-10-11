// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// stroke type
enum TYPE
{
    WAIT,
    LINEAR,
    POLAR
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
typedef std::vector<STROKE> KAKIJUN[80];

extern KAKIJUN g_kanji1_kakijun;
extern INT g_map[80];

VOID InitKanji1(VOID);

// 書き順イメージのサイズ。
#define KAKIJUN_WIDTH 254
#define KAKIJUN_HEIGHT 254
// 書き順イメージの中心点。
#define KAKIJUN_CENTER_X (KAKIJUN_WIDTH / 2)
#define KAKIJUN_CENTER_Y (KAKIJUN_HEIGHT / 2)
