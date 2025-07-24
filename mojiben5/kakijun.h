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
struct GA
{
    TYPE type;
    INT angle0;
    INT angle1;
    INT cx;
    INT cy;

    // Win32 Region data
    DWORD cb;
    const BYTE *pb;
};
typedef std::vector<GA> KAKIJUN[160];

extern KAKIJUN g_kanji2_kakijun;
extern INT g_map[160];

VOID InitKanji2(VOID);
