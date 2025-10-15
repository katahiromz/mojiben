#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>

#define PRINTF
//#define PRINTF printf

// ランデータの安全な追加
inline void SerializeRun(std::vector<WORD>& out, WORD start, WORD length)
{
    PRINTF("Run: start=%d, length=%d\n", start, length);
    out.push_back(start);
    out.push_back(length);
}

BOOL SerializeRegion(std::vector<WORD>& out, HRGN hRgn)
{
    RECT rc;
    if (GetRgnBox(hRgn, &rc) == NULLREGION)
        return FALSE;

    INT x = rc.left;
    INT y = rc.top;
    INT cx = rc.right - rc.left;
    INT cy = rc.bottom - rc.top;

    // サイズが負の場合やゼロの場合のチェック
    if (cx <= 0 || cy <= 0)
        return FALSE;

    BOOL wide = (cx >= cy);
    BYTE signature = wide ? 'H' : 'V';

    // ヘッダー情報
    out.push_back((WORD)signature);
    out.push_back((WORD)x);
    out.push_back((WORD)y);
    out.push_back((WORD)cx);
    out.push_back((WORD)cy);

    if (wide)
    {
        // 水平スキャン：各行を処理
        for (INT y0 = y; y0 < y + cy; ++y0)
        {
            INT runStart = -1;  // 現在のランの開始位置

            for (INT x0 = x; x0 < x + cx; ++x0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // 新しいランの開始
                    {
                        runStart = x0;
                    }
                }
                else
                {
                    if (runStart != -1)  // ランの終了
                    {
                        SerializeRun(out, runStart - x, x0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // 行の終端でランが続いている場合
            if (runStart != -1)
            {
                SerializeRun(out, runStart - x, (x + cx) - runStart);
            }

            // 行の終わりマーカー
            out.push_back(0xFFFF);
        }
    }
    else
    {
        // 垂直スキャン：各列を処理
        for (INT x0 = x; x0 < x + cx; ++x0)
        {
            INT runStart = -1;  // 現在のランの開始位置

            for (INT y0 = y; y0 < y + cy; ++y0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // 新しいランの開始
                    {
                        runStart = y0;
                    }
                }
                else
                {
                    if (runStart != -1)  // ランの終了
                    {
                        SerializeRun(out, runStart - y, y0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // 列の終端でランが続いている場合
            if (runStart != -1)
            {
                SerializeRun(out, runStart - y, (y + cy) - runStart);
            }

            // 列の終わりマーカー
            out.push_back(0xFFFF);
        }
    }

    return TRUE;
}

HRGN DeserializeRegion(const WORD *pw, size_t size)
{
    if (size < 5)
    {
        PRINTF("Error: Insufficient data size\n");
        return NULL;
    }

    WORD signature = pw[0];
    BOOL isHorizontal = (signature == 'H');
    size_t offset = 1;

    if (signature != 'H' && signature != 'V')
    {
        PRINTF("Error: Invalid signature\n");
        return NULL;
    }

    // 座標とサイズの読み取り
    INT x = (INT)pw[offset];
    INT y = (INT)pw[offset + 1];
    INT cx = (INT)pw[offset + 2];
    INT cy = (INT)pw[offset + 3];
    offset += 4;

    PRINTF("Deserializing: signature=%c, bounds=(%d,%d,%dx%d)\n",
           signature, x, y, cx, cy);

    // サイズの妥当性チェック
    if (cx <= 0 || cy <= 0)
    {
        PRINTF("Error: Invalid region size\n");
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
        while (offset < size && yCurrent < y + cy)
        {
            // 各行のランデータを処理
            while (offset + 1 < size)
            {
                WORD value = pw[offset];

                // 行終了マーカーのチェック
                if (value == 0xFFFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pw[offset];
                DWORD runLength = pw[offset + 1];
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
        while (offset < size && xCurrent < x + cx)
        {
            // 各列のランデータを処理
            while (offset + 1 < size)
            {
                WORD value = pw[offset];

                // 列終了マーカーのチェック
                if (value == 0xFFFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pw[offset];
                DWORD runLength = pw[offset + 1];
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

// ランデータの安全な追加 (254版)
inline void SerializeRun254(std::vector<BYTE>& out, BYTE start, BYTE length)
{
    PRINTF("Run: start=%d, length=%d\n", start, length);
    out.push_back(start);
    out.push_back(length);
}

BOOL SerializeRegion254(std::vector<BYTE>& out, HRGN hRgn)
{
    RECT rc;
    if (GetRgnBox(hRgn, &rc) == NULLREGION)
        return FALSE;

    INT x = rc.left;
    INT y = rc.top;
    INT cx = rc.right - rc.left;
    INT cy = rc.bottom - rc.top;

    // サイズが負の場合やゼロの場合のチェック
    if (cx <= 0 || cy <= 0 || cx > 254 || cy > 254)
        return FALSE;
    if (x <= 0 || y <= 0 || x > 254 || y > 254)
        return FALSE;

    BOOL wide = (cx >= cy);
    BYTE signature = wide ? 'h' : 'v';

    // ヘッダー情報
    out.push_back((BYTE)signature);
    out.push_back((BYTE)x);
    out.push_back((BYTE)y);
    out.push_back((BYTE)cx);
    out.push_back((BYTE)cy);

    if (wide)
    {
        // 水平スキャン：各行を処理
        for (INT y0 = y; y0 < y + cy; ++y0)
        {
            INT runStart = -1;  // 現在のランの開始位置

            for (INT x0 = x; x0 < x + cx; ++x0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // 新しいランの開始
                    {
                        runStart = x0;
                    }
                }
                else
                {
                    if (runStart != -1)  // ランの終了
                    {
                        SerializeRun254(out, runStart - x, x0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // 行の終端でランが続いている場合
            if (runStart != -1)
            {
                SerializeRun254(out, runStart - x, (x + cx) - runStart);
            }

            // 行の終わりマーカー
            out.push_back(0xFF);
        }
    }
    else
    {
        // 垂直スキャン：各列を処理
        for (INT x0 = x; x0 < x + cx; ++x0)
        {
            INT runStart = -1;  // 現在のランの開始位置

            for (INT y0 = y; y0 < y + cy; ++y0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // 新しいランの開始
                    {
                        runStart = y0;
                    }
                }
                else
                {
                    if (runStart != -1)  // ランの終了
                    {
                        SerializeRun254(out, runStart - y, y0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // 列の終端でランが続いている場合
            if (runStart != -1)
            {
                SerializeRun254(out, runStart - y, (y + cy) - runStart);
            }

            // 列の終わりマーカー
            out.push_back(0xFF);
        }
    }

    return TRUE;
}

HRGN DeserializeRegion254(const BYTE *pb, size_t size)
{
    if (size < 5)
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
    INT x = (INT)pb[offset];
    INT y = (INT)pb[offset + 1];
    INT cx = (INT)pb[offset + 2];
    INT cy = (INT)pb[offset + 3];
    offset += 4;

    PRINTF("Deserializing: signature=%c, bounds=(%d,%d,%dx%d)\n",
           signature, x, y, cx, cy);

    // サイズの妥当性チェック
    if (cx <= 0 || cy <= 0)
    {
        PRINTF("Error: Invalid region size\n");
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
        while (offset < size && yCurrent < y + cy)
        {
            // 各行のランデータを処理
            while (offset + 1 < size)
            {
                BYTE value = pb[offset];

                // 行終了マーカーのチェック
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= size)
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
        while (offset < size && xCurrent < x + cx)
        {
            // 各列のランデータを処理
            while (offset + 1 < size)
            {
                BYTE value = pb[offset];

                // 列終了マーカーのチェック
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // ランの開始位置とレングスを取得
                if (offset + 1 >= size)
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

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, FAR * LPBITMAPINFOEX;

static HBITMAP LoadBitmapFromFile(LPCTSTR pszFileName)
{
    return (HBITMAP)LoadImage(NULL, pszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

static BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm)
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

static HBITMAP CreateBitmapFromRegionGeneric(HRGN hRgn, INT size)
{
    HDC hdc = CreateCompatibleDC(NULL);
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = size;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
    HGDIOBJ hbmOld = SelectObject(hdc, hbm);
    RECT rc = { 0, 0, size, size };
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRgn(hdc, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SelectObject(hdc, hbmOld);
    DeleteDC(hdc);
    return hbm;
}

static HBITMAP CreateBitmapFromRegion(HRGN hRgn)
{
    return CreateBitmapFromRegionGeneric(hRgn, 300);
}

static HBITMAP CreateBitmapFromRegion254(HRGN hRgn)
{
    return CreateBitmapFromRegionGeneric(hRgn, 254);
}

static HRGN CreateRegionFromBitmapGeneric(HBITMAP hbm, INT size)
{
    HRGN hRgn1 = CreateRectRgn(0, 0, 0, 0);
    HDC hdc = CreateCompatibleDC(NULL);
    HGDIOBJ hbmOld = SelectObject(hdc, hbm);
    for (INT y = 0; y < size; y++)
    {
        for (INT x = 0; x < size; x++)
        {
            if (!GetPixel(hdc, x, y))
            {
                HRGN hRgn2 = CreateRectRgn(x, y, x + 1, y + 1);
                CombineRgn(hRgn1, hRgn1, hRgn2, RGN_OR);
                DeleteObject(hRgn2);
            }
        }
    }
    SelectObject(hdc, hbmOld);
    DeleteDC(hdc);
    return hRgn1;
}

static HRGN CreateRegionFromBitmap(HBITMAP hbm)
{
    return CreateRegionFromBitmapGeneric(hbm, 300);
}

static HRGN CreateRegionFromBitmap254(HBITMAP hbm)
{
    return CreateRegionFromBitmapGeneric(hbm, 254);
}
