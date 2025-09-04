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
typedef std::vector<STROKE> KAKIJUN[160];

extern KAKIJUN g_kanji2_kakijun;
extern INT g_map[160];

VOID InitKanji2(VOID);
