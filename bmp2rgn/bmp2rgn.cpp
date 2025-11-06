// bmp2rgn.cpp --- Convert BMP to RGN data
// License: MIT
#include <windows.h>
#include <stdio.h>
#include <vector>

void version() {
    puts("0.0");
}

void usage() {
    puts("Usage: bmp2rgn input.bmp");
}

HBITMAP LoadBitmapFromFile(LPCTSTR pszFileName)
{
    return (HBITMAP)LoadImage(NULL, pszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

#define PRINTF

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

    INT x = rc.left, y = rc.top;
    INT cx = rc.right - rc.left, cy = rc.bottom - rc.top;

    if (cx <= 0 || cy <= 0 || cx > 254 || cy > 254)
        return FALSE;
    if (x < 0 || y < 0 || x >= 254 || y >= 254)
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

HRGN CreateRegionFromBitmapGeneric(HBITMAP hbm, INT cxy)
{
    HRGN hRgn1 = CreateRectRgn(0, 0, 0, 0);
    HDC hdc = CreateCompatibleDC(NULL);
    HGDIOBJ hbmOld = SelectObject(hdc, hbm);
    for (INT y = 0; y < cxy; y++)
    {
        for (INT x = 0; x < cxy; x++)
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

HRGN CreateRegionFromBitmap254(HBITMAP hbm)
{
    return CreateRegionFromBitmapGeneric(hbm, 254);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage();
        return 1;
    }

    HBITMAP hbm = LoadBitmapFromFile(argv[1]);
    if (!hbm) {
        printf("Cannot load bmp '%s'\n", argv[1]);
        return 1;
    }

    HRGN hRgn = CreateRegionFromBitmap254(hbm);
    if (!hRgn) {
        printf("Internal error #1\n");
        return 1;
    }

    std::vector<BYTE> out;
    if (!SerializeRegion254(out, hRgn)) {
        printf("Internal error #2\n");
        return 1;
    }

    char filename[MAX_PATH];
    lstrcpyA(filename, argv[1]);
    lstrcatA(filename, ".rgn");

    FILE *fout = fopen(filename, "wb");
    if (!fout) {
        printf("Cannot write file '%s'\n", filename);
        return 1;
    }

    fwrite(&out[0], 1, out.size(), fout);
    fclose(fout);

    DeleteObject(hRgn);
    DeleteObject(hbm);
    return 0;
}
