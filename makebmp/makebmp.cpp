// makebmp.cpp - Create a bitmap of single character
// License: MIT
#include <stdio.h>
#include <windows.h>

void version() {
    puts("0.0");
}

void usage() {
    puts("Usage: makebmp character [\"font name\" [size]]");
}

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

int main(int argc, char **argv) {
    for (int iarg = 1; iarg < argc; ++iarg) {
        const char *arg = argv[iarg];
        if (lstrcmpiA(arg, "--help") == 0) {
            usage();
            return 1;
        }
        if (lstrcmpiA(arg, "--version") == 0) {
            version();
            return 1;
        }
    }

    if (argc <= 1 || argc > 4) {
        usage();
        return 1;
    }

    const char *moji = argv[1];
    int size = 254;
    wchar_t font_name[LF_FACESIZE] = L"モトヤ教科書3等幅";

    if (argc >= 3)
        MultiByteToWideChar(CP_ACP, 0, argv[2], -1, font_name, ARRAYSIZE(font_name));
    if (argc >= 4)
        size = atoi(argv[3]);

    BITMAPINFO bi;
    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = size;
    bi.bmiHeader.biHeight       = size;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 24;

    LOGFONTW lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -size;
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lf.lfQuality = PROOF_QUALITY;
    lf.lfWeight = FW_NORMAL;
    lstrcpyW(lf.lfFaceName, font_name);
    HFONT hFont = CreateFontIndirectW(&lf);

    LPVOID pvBits;
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    HGDIOBJ hbmOld = SelectObject(hdc, hbm);
    HGDIOBJ hFontOld = SelectObject(hdc, hFont);
    RECT rc = { 0, 0, size, size };
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, 0);
    DrawTextA(hdc, moji, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
    SelectObject(hdc, hFontOld);
    SelectObject(hdc, hbmOld);

    CHAR filename[64];
    lstrcpyA(filename, moji);
    lstrcatA(filename, ".bmp");

    SaveBitmapToFile(filename, hbm);

    DeleteObject(hFont);
    DeleteObject(hbm);
    DeleteDC(hdc);
    return 0;
}
