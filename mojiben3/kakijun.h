// Moji No Benkyo (3)
// Copyright (C) 2019-2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// stroke type
enum STROKE_TYPE
{
    WAIT,
    LINEAR,
    POLAR
};

// stroke info
struct STROKE
{
    STROKE_TYPE type;
    INT angle0;
    INT angle1;
    INT cx;
    INT cy;

    // Win32 Region data
    DWORD cb;
    const BYTE *pb;
};
typedef std::vector<STROKE> KAKIJUN[10];

extern KAKIJUN g_digits_kakijun;

VOID InitDigits(VOID);

// 書き順イメージのサイズ。
#define KAKIJUN_SIZE 300
// 書き順イメージの中心点。
#define KAKIJUN_CENTER_XY (KAKIJUN_SIZE / 2)
