// Moji No Benkyo (3)
// Copyright (C) 2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// Japanese, Shift_JIS
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

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

#define WIDTH 300
#define HEIGHT 300

static char g_aszReadings[][64] =
{
    "0:�[���A(�ꂢ)",
    "1:����",
    "2:��",
    "3:����",
    "4:���",
    "5:��",
    "6:�낭",
    "7:�Ȃ�",
    "8:�͂�",
    "9:���イ",
    "10:���イ",
    "20:�ɂ��イ",
    "30:���񂶂イ",
    "40:��񂶂イ",
    "50:�����イ",
    "60:�낭���イ",
    "70:�ȂȂ��イ",
    "80:�͂����イ",
    "90:���イ���イ",
    "100:�ЂႭ",
    "200:�ɂЂႭ",
    "300:����тႭ",
    "400:���ЂႭ",
    "500:���ЂႭ",
    "600:����҂Ⴍ",
    "700:�ȂȂЂႭ",
    "800:�͂��҂Ⴍ",
    "900:���イ�ЂႭ",
    "1000:����",
    "2000:�ɂ���",
    "3000:���񂺂�",
    "4000:��񂹂�",
    "5000:������",
    "6000:�낭����",
    "7000:�ȂȂ���",
    "8000:�͂�����",
    "9000:���イ����",
    "10000:�����܂�",
    "20000:�ɂ܂�",
    "30000:����܂�",
    NULL,
};

int main(void)
{
    HBITMAP hbm;
    HDC hdc;
    HGDIOBJ hbmOld, hFontOld;
    HFONT hFont;
    LOGFONT lf;
    BITMAPINFO bi;
    LPVOID pvBits;
    INT i;
    CHAR sz2[32];

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = WIDTH;
    bi.bmiHeader.biHeight       = HEIGHT;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 24;
    
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -HEIGHT / 3;
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lf.lfQuality = ANTIALIASED_QUALITY;
    lf.lfWeight = FW_BOLD;
    lstrcpy(lf.lfFaceName, "���g�����ȏ�3����");
    hFont = CreateFontIndirect(&lf);
    hdc = CreateCompatibleDC(NULL);
    hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    hFontOld = SelectObject(hdc, hFont);

    for (i = 0; g_aszReadings[i]; ++i)
    {
        wsprintfA(sz2, "digit%03d.bmp", i);
        hbmOld = SelectObject(hdc, hbm);
        RECT rc = { 0, 0, WIDTH, HEIGHT };
        FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
        char *pch = strchr(g_aszReadings[i], ':');
        *pch = 0;
        DrawTextA(hdc, g_aszReadings[i], -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hbmOld);
        SaveBitmapToFile(sz2, hbm);
    }
    SelectObject(hdc, hFontOld);

    DeleteObject(hbm);
    DeleteObject(hFont);
    DeleteDC(hdc);
    return 0;
}
