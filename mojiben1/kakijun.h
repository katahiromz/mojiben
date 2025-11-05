// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// stroke info
struct STROKE {
    enum TYPE {
        WAIT,
        LINEAR,
        POLAR,
        DOT
    };
    TYPE type;
    INT angle0;
    INT angle1;
    INT cx;     // width
    INT cy;     // height
    INT res;
};
typedef std::vector<std::vector<STROKE> > KAKIJUN;

// 書き順イメージのサイズ。
#define KAKIJUN_SIZE 254
// 書き順イメージの中心点。
#define KAKIJUN_CENTER_XY (KAKIJUN_SIZE / 2)
