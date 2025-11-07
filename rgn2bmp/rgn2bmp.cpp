// rgn2bmp.cpp --- Convert RGN data to BMP
// License: MIT
#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "../read_all.h"

void version() {
    puts("0.0");
}

void usage() {
    puts("Usage: rgn2bmp input.rgn");
}

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, FAR * LPBITMAPINFOEX;

BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm)
{
    BOOL f;
    DWORD dwError = NO_ERROR;
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

#define PRINTF

HRGN DeserializeRegion254(const BYTE *pb, size_t data_size)
{
    if (data_size < 5)
    {
        PRINTF("Error: Insufficient data size\n");
        return NULL;
    }

    BYTE signature = pb[0];
    BOOL isHorizontal = (signature == 'h');
    size_t offset = 1;

    if (signature != 'h' && signature != 'v')
    {
        PRINTF("Error: Invalid signature\n");
        return NULL;
    }

    // 座標とサイズの読み取り
    INT x = pb[offset], y = pb[offset + 1];
    INT cx = pb[offset + 2], cy = pb[offset + 3];
    offset += 4;

    PRINTF("Deserializing: signature=%c, bounds=(%d,%d,%dx%d)\n",
           signature, x, y, cx, cy);

    if (cx <= 0 || cy <= 0 || cx > 254 || cy > 254)
    {
        PRINTF("Error: Invalid region size\n");
        return NULL;
    }
    if (x < 0 || y < 0 || x >= 254 || y >= 254)
    {
        PRINTF("Error: Invalid region\n");
        return NULL;
    }

    // RGNDATA構造体の準備
    RGNDATAHEADER header = { sizeof(header) };
    header.iType = RDH_RECTANGLES;
    SetRect(&header.rcBound, x, y, x + cx, y + cy);

    std::vector<RECT> rects;

    if (isHorizontal)
    {
        INT yCurrent = y;
        while (offset < data_size && yCurrent < y + cy)
        {
            // 各行のランデータを処理
            while (offset + 1 < data_size)
            {
                BYTE value = pb[offset];

                // 行終了マーカーのチェック
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= data_size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pb[offset];
                DWORD runLength = pb[offset + 1];
                offset += 2;

                PRINTF("H-Run: start=%d, length=%d at y=%d\n", runStart, runLength, yCurrent);

                // 境界チェック
                if (runStart + runLength > (DWORD)cx)
                {
                    PRINTF("Warning: Run exceeds horizontal bounds\n");
                    runLength = cx - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        x + (INT)runStart,
                        yCurrent,
                        x + (INT)runStart + (INT)runLength,
                        yCurrent + 1
                    };
                    rects.push_back(r);
                }
            }
            yCurrent++;
        }
    }
    else
    {
        INT xCurrent = x;
        while (offset < data_size && xCurrent < x + cx)
        {
            // 各列のランデータを処理
            while (offset + 1 < data_size)
            {
                BYTE value = pb[offset];

                // 列終了マーカーのチェック
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= data_size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pb[offset];
                DWORD runLength = pb[offset + 1];
                offset += 2;

                PRINTF("V-Run: start=%d, length=%d at x=%d\n", runStart, runLength, xCurrent);

                // 境界チェック
                if (runStart + runLength > (DWORD)cy)
                {
                    PRINTF("Warning: Run exceeds vertical bounds\n");
                    runLength = cy - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        xCurrent,
                        y + (INT)runStart,
                        xCurrent + 1,
                        y + (INT)runStart + (INT)runLength
                    };
                    rects.push_back(r);
                }
            }
            xCurrent++;
        }
    }

    if (rects.empty())
    {
        PRINTF("Warning: No rectangles generated\n");
        return NULL;
    }

    // RECTを元にリージョンを作成
    header.nCount = (DWORD)rects.size();
    size_t dataSize = sizeof(RGNDATAHEADER) + rects.size() * sizeof(RECT);
    RGNDATA* rgndata = (RGNDATA*)malloc(dataSize);

    if (!rgndata)
    {
        PRINTF("Error: Memory allocation failed\n");
        return NULL;
    }

    memcpy(&rgndata->rdh, &header, sizeof(RGNDATAHEADER));
    memcpy(rgndata->Buffer, &rects[0], rects.size() * sizeof(RECT));

    HRGN hRgn = ExtCreateRegion(NULL, (DWORD)dataSize, rgndata);
    free(rgndata);

    PRINTF("Created region with %u rectangles\n", (UINT)rects.size());
    return hRgn;
}

HBITMAP CreateBitmapFromRegionGeneric(HRGN hRgn, INT cxy)
{
    HDC hdc = CreateCompatibleDC(NULL);
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cxy;
    bmi.bmiHeader.biHeight = cxy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
    HGDIOBJ hbmOld = SelectObject(hdc, hbm);
    RECT rc = { 0, 0, cxy, cxy };
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRgn(hdc, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SelectObject(hdc, hbmOld);
    DeleteDC(hdc);
    return hbm;
}

HBITMAP CreateBitmapFromRegion254(HRGN hRgn)
{
    return CreateBitmapFromRegionGeneric(hRgn, 254);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage();
        return 1;
    }

    std::string binary;
    if (!read_all(binary, argv[1])) {
        printf("Cannot open '%s'\n", argv[1]);
        return 1;
    }

    HRGN hRgn = DeserializeRegion254((PBYTE)&binary[0], binary.size());
    if (!hRgn) {
        printf("Invalid format: '%s'\n", argv[1]);
        return 1;
    }

    HBITMAP hbm = CreateBitmapFromRegion254(hRgn);

    CHAR filename[MAX_PATH];
    lstrcpyA(filename, argv[1]);
    lstrcatA(filename, ".bmp");
    if (!SaveBitmapToFile(filename, hbm)) {
        printf("Cannot write: '%s'\n", filename);
        return 1;
    }

    DeleteObject(hRgn);
    DeleteObject(hbm);
    return 0;
}
