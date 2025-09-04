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
typedef std::vector<STROKE> KAKIJUN[200];

extern KAKIJUN g_kanji3_kakijun;
extern INT g_map[200];

VOID InitKanji3(VOID);
