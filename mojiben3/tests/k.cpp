// Moji No Benkyo (3)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// Japanese, Shift_JIS
#include <windows.h>
#include <stdio.h>

#include <map>
#include <vector>
using namespace std;

#include "kakijun.h"

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, FAR * LPBITMAPINFOEX;

BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm)
{
    BOOL f;
    DWORD dwError;
    BITMAPFILEHEADER bf;
    BITMAPINFOEX bi;
    BITMAPINFOHEADER *pbmih;
    DWORD cb;
    DWORD cColors, cbColors;
    HDC hDC;
    HANDLE hFile;
    LPVOID pBits;
    BITMAP bm;

    if (!GetObject(hbm, sizeof(BITMAP), &bm))
        return FALSE;

    pbmih = &bi.bmiHeader;
    ZeroMemory(pbmih, sizeof(BITMAPINFOHEADER));
    pbmih->biSize             = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth            = bm.bmWidth;
    pbmih->biHeight           = bm.bmHeight;
    pbmih->biPlanes           = 1;
    pbmih->biBitCount         = bm.bmBitsPixel;
    pbmih->biCompression      = BI_RGB;
    pbmih->biSizeImage        = bm.bmWidthBytes * bm.bmHeight;

    if (bm.bmBitsPixel < 16)
        cColors = 1 << bm.bmBitsPixel;
    else
        cColors = 0;
    cbColors = cColors * sizeof(RGBQUAD);

    bf.bfType = 0x4d42;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    cb = sizeof(BITMAPFILEHEADER) + pbmih->biSize + cbColors;
    bf.bfOffBits = cb;
    bf.bfSize = cb + pbmih->biSizeImage;

    pBits = HeapAlloc(GetProcessHeap(), 0, pbmih->biSizeImage);
    if (pBits == NULL)
        return FALSE;

    f = FALSE;
    hDC = GetDC(NULL);
    if (hDC != NULL)
    {
        if (GetDIBits(hDC, hbm, 0, bm.bmHeight, pBits, (BITMAPINFO*)&bi,
            DIB_RGB_COLORS))
        {
            hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL |
                               FILE_FLAG_WRITE_THROUGH, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                f = WriteFile(hFile, &bf, sizeof(BITMAPFILEHEADER), &cb, NULL) &&
                    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &cb, NULL) &&
                    WriteFile(hFile, bi.bmiColors, cbColors, &cb, NULL) &&
                    WriteFile(hFile, pBits, pbmih->biSizeImage, &cb, NULL);
                if (!f)
                    dwError = GetLastError();
                CloseHandle(hFile);

                if (!f)
                    DeleteFile(pszFileName);
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
        ReleaseDC(NULL, hDC);
    }
    else
        dwError = GetLastError();

    HeapFree(GetProcessHeap(), 0, pBits);
    SetLastError(dwError);
    return f;
}

#define SIZE 300

VOID DoIt(INT n, const char *name, HDC hdc, HBITMAP hbm)
{
    CHAR sz[1024];
    vector<GA> v;
    HGDIOBJ hbmOld;
    RECT rc;
    HRGN hRgn, hRgn2;

    v = g_katakana_kakijun[n];
    hRgn2 = CreateRectRgn(0, 0, 0, 0);
    for(UINT i = 0; i < v.size(); i++)
    {
        hRgn = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
        CombineRgn(hRgn2, hRgn2, hRgn, RGN_OR);
        DeleteObject(hRgn);
    }

    hbmOld = SelectObject(hdc, hbm);
    rc.left = rc.top = 0;
    rc.right = rc.bottom = 300;
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRgn(hdc, hRgn2, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SelectObject(hdc, hbmOld);

    wsprintf(sz, "KataL\\%sL.bmp", name);
    SaveBitmapToFile(sz, hbm);
}

int main(void)
{
    HBITMAP hbm;
    HDC hdc;
    BITMAPINFO bi;
    LPVOID pvBits;
    
    InitKatakana();

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = SIZE;
    bi.bmiHeader.biHeight       = SIZE;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 24;
    hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    hdc = CreateCompatibleDC(NULL);
    DoIt(0, "ア", hdc, hbm);
    DoIt(1, "イ", hdc, hbm);
    DoIt(2, "ウ", hdc, hbm);
    DoIt(3, "エ", hdc, hbm);
    DoIt(4, "オ", hdc, hbm);
    DoIt(10, "カ", hdc, hbm);
    DoIt(11, "キ", hdc, hbm);
    DoIt(12, "ク", hdc, hbm);
    DoIt(13, "ケ", hdc, hbm);
    DoIt(14, "コ", hdc, hbm);
    DoIt(20, "サ", hdc, hbm);
    DoIt(21, "シ", hdc, hbm);
    DoIt(22, "ス", hdc, hbm);
    DoIt(23, "セ", hdc, hbm);
    DoIt(24, "ソ", hdc, hbm);
    DoIt(30, "タ", hdc, hbm);
    DoIt(31, "チ", hdc, hbm);
    DoIt(32, "ツ", hdc, hbm);
    DoIt(33, "テ", hdc, hbm);
    DoIt(34, "ト", hdc, hbm);
    DoIt(40, "ナ", hdc, hbm);
    DoIt(41, "ニ", hdc, hbm);
    DoIt(42, "ヌ", hdc, hbm);
    DoIt(43, "ネ", hdc, hbm);
    DoIt(44, "ノ", hdc, hbm);
    DoIt(50, "ハ", hdc, hbm);
    DoIt(51, "ヒ", hdc, hbm);
    DoIt(52, "フ", hdc, hbm);
    DoIt(53, "ヘ", hdc, hbm);
    DoIt(54, "ホ", hdc, hbm);
    DoIt(60, "マ", hdc, hbm);
    DoIt(61, "ミ", hdc, hbm);
    DoIt(62, "ム", hdc, hbm);
    DoIt(63, "メ", hdc, hbm);
    DoIt(64, "モ", hdc, hbm);
    DoIt(70, "ヤ", hdc, hbm);
    DoIt(72, "ユ", hdc, hbm);
    DoIt(74, "ヨ", hdc, hbm);
    DoIt(80, "ラ", hdc, hbm);
    DoIt(81, "リ", hdc, hbm);
    DoIt(82, "ル", hdc, hbm);
    DoIt(83, "レ", hdc, hbm);
    DoIt(84, "ロ", hdc, hbm);
    DoIt(90, "ワ", hdc, hbm);
    DoIt(94, "ヲ", hdc, hbm);
    DoIt(104, "ン", hdc, hbm);
    DeleteDC(hdc);
    DeleteObject(hbm);
    return 0;
}
