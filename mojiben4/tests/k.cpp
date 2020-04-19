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
    DoIt(0, "�A", hdc, hbm);
    DoIt(1, "�C", hdc, hbm);
    DoIt(2, "�E", hdc, hbm);
    DoIt(3, "�G", hdc, hbm);
    DoIt(4, "�I", hdc, hbm);
    DoIt(10, "�J", hdc, hbm);
    DoIt(11, "�L", hdc, hbm);
    DoIt(12, "�N", hdc, hbm);
    DoIt(13, "�P", hdc, hbm);
    DoIt(14, "�R", hdc, hbm);
    DoIt(20, "�T", hdc, hbm);
    DoIt(21, "�V", hdc, hbm);
    DoIt(22, "�X", hdc, hbm);
    DoIt(23, "�Z", hdc, hbm);
    DoIt(24, "�\", hdc, hbm);
    DoIt(30, "�^", hdc, hbm);
    DoIt(31, "�`", hdc, hbm);
    DoIt(32, "�c", hdc, hbm);
    DoIt(33, "�e", hdc, hbm);
    DoIt(34, "�g", hdc, hbm);
    DoIt(40, "�i", hdc, hbm);
    DoIt(41, "�j", hdc, hbm);
    DoIt(42, "�k", hdc, hbm);
    DoIt(43, "�l", hdc, hbm);
    DoIt(44, "�m", hdc, hbm);
    DoIt(50, "�n", hdc, hbm);
    DoIt(51, "�q", hdc, hbm);
    DoIt(52, "�t", hdc, hbm);
    DoIt(53, "�w", hdc, hbm);
    DoIt(54, "�z", hdc, hbm);
    DoIt(60, "�}", hdc, hbm);
    DoIt(61, "�~", hdc, hbm);
    DoIt(62, "��", hdc, hbm);
    DoIt(63, "��", hdc, hbm);
    DoIt(64, "��", hdc, hbm);
    DoIt(70, "��", hdc, hbm);
    DoIt(72, "��", hdc, hbm);
    DoIt(74, "��", hdc, hbm);
    DoIt(80, "��", hdc, hbm);
    DoIt(81, "��", hdc, hbm);
    DoIt(82, "��", hdc, hbm);
    DoIt(83, "��", hdc, hbm);
    DoIt(84, "��", hdc, hbm);
    DoIt(90, "��", hdc, hbm);
    DoIt(94, "��", hdc, hbm);
    DoIt(104, "��", hdc, hbm);
    DeleteDC(hdc);
    DeleteObject(hbm);
    return 0;
}
