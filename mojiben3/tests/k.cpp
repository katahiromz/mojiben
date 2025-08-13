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
    "0:É[ÉçÅA(ÇÍÇ¢)",
    "1:Ç¢Çø",
    "2:Ç…",
    "3:Ç≥ÇÒ",
    "4:ÇÊÇÒ",
    "5:Ç≤",
    "6:ÇÎÇ≠",
    "7:Ç»Ç»",
    "8:ÇÕÇø",
    "9:Ç´Ç„Ç§",
    "10:Ç∂Ç„Ç§",
    "20:Ç…Ç∂Ç„Ç§",
    "30:Ç≥ÇÒÇ∂Ç„Ç§",
    "40:ÇÊÇÒÇ∂Ç„Ç§",
    "50:Ç≤Ç∂Ç„Ç§",
    "60:ÇÎÇ≠Ç∂Ç„Ç§",
    "70:Ç»Ç»Ç∂Ç„Ç§",
    "80:ÇÕÇøÇ∂Ç„Ç§",
    "90:Ç´Ç„Ç§Ç∂Ç„Ç§",
    "100:Ç–Ç·Ç≠",
    "200:Ç…Ç–Ç·Ç≠",
    "300:Ç≥ÇÒÇ—Ç·Ç≠",
    "400:ÇÊÇÒÇ–Ç·Ç≠",
    "500:Ç≤Ç–Ç·Ç≠",
    "600:ÇÎÇ¡Ç“Ç·Ç≠",
    "700:Ç»Ç»Ç–Ç·Ç≠",
    "800:ÇÕÇ¡Ç“Ç·Ç≠",
    "900:Ç´Ç„Ç§Ç–Ç·Ç≠",
    "1000:ÇπÇÒ",
    "2000:Ç…ÇπÇÒ",
    "3000:Ç≥ÇÒÇ∫ÇÒ",
    "4000:ÇÊÇÒÇπÇÒ",
    "5000:Ç≤ÇπÇÒ",
    "6000:ÇÎÇ≠ÇπÇÒ",
    "7000:Ç»Ç»ÇπÇÒ",
    "8000:ÇÕÇ¡ÇπÇÒ",
    "9000:Ç´Ç„Ç§ÇπÇÒ",
    "10000:Ç¢ÇøÇ‹ÇÒ",
    "20000:Ç…Ç‹ÇÒ",
    "30000:Ç≥ÇÒÇ‹ÇÒ",
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
    lstrcpy(lf.lfFaceName, "ÉÇÉgÉÑã≥â»èë3ìôïù");
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
