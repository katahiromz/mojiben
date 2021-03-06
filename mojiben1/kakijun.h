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
    INT cx;     // width
    INT cy;     // height

    // Win32 Region data
    DWORD cb;
    const BYTE *pb;
};
typedef std::map<INT, std::vector<GA> > KAKIJUN;

extern KAKIJUN g_hiragana_kakijun;
extern KAKIJUN g_katakana_kakijun;

VOID InitHiragana(VOID);
VOID InitKatakana(VOID);
