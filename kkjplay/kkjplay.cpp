// kkjplay.cpp --- KKJ (kakijun) Player
// License: MIT
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <cmath>
#include <process.h>
#include <assert.h>
#include "../kakijun.h"
#include "../read_all.h"
#include "../CGdiObj.h"
#include "../mstr.h"

HINSTANCE g_hInstance = NULL;
HWND g_hMainWnd = NULL;
HBITMAP g_hbmKakijun = NULL; // week ref
HBRUSH g_hbrRed = NULL;
BOOL g_bPlaying = FALSE;
INT g_iFrame = -1;

std::vector<STROKE> g_kkj;
std::vector<std::wstring> g_rgn_files;

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

HRGN MyCreateRegion(std::vector<STROKE>& v, std::wstring file) {
    std::string binary;
    read_all(binary, file.c_str());
    return DeserializeRegion254((PBYTE)binary.c_str(), (DWORD)binary.size());
}

void OnDraw(HWND hwnd, HDC hdc, RECT& rc) {
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));

    if (g_iFrame == -1 && g_hbmKakijun) {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HGDIOBJ hbmOld = SelectObject(hdcMem, g_hbmKakijun);
        BitBlt(hdc, 0, 0, 254, 254, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
        DeleteDC(hdcMem);

        SetDlgItemTextW(hwnd, stc1, NULL);
    } else if (g_iFrame != -1) {
        SetRect(&rc, 0, 0, 254, 254);
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

        if (0 <= g_iFrame && g_iFrame < (INT)g_rgn_files.size()) {
            HRGN hRgn = MyCreateRegion(g_kkj, g_rgn_files[g_iFrame]);
            FillRgn(hdc, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
            DeleteObject(hRgn);
            SetDlgItemInt(hwnd, stc1, g_iFrame, FALSE);
        } else {
            SetDlgItemTextW(hwnd, stc1, NULL);
        }
    } else if (g_kkj.size()) {
        SetRect(&rc, 0, 0, 254, 254);
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

        std::vector<STROKE>& v = g_kkj;
        CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
        INT ires = 0;
        for (UINT i = 0; i < v.size(); i++)
        {
            if (v[i].type != STROKE::WAIT) {
                CRgn hRgn2(MyCreateRegion(v, g_rgn_files[ires++]));
                CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
            }
        }
        ires = 0;

        FillRgn(hdc, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetDlgItemTextW(hwnd, stc1, NULL);
    } else {
        SetRect(&rc, 0, 0, 254, 254);
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        DrawTextW(hdc, L"Drop a KKJ file", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
        SetDlgItemTextW(hwnd, stc1, NULL);
    }
}

static unsigned ThreadProcWorker(void) {
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    INT k;
    POINT apt[5];

    SetRect(&rc, 0, 0, 254, 254);
    siz.cx = siz.cy = 254;

    std::vector<STROKE>& v = g_kkj;

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    INT ires = 0;
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].type != STROKE::WAIT) {
            CRgn hRgn2(MyCreateRegion(v, g_rgn_files[ires++]));
            CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
        }
    }
    ires = 0;

    {
        CDC hdc(g_hMainWnd);
        CDC hdcMem(hdc);
        hbm1 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbm2 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);

        hbmOld = SelectObject(hdcMem, hbm1);
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SelectObject(hdcMem, hbmOld);
    }

    g_hbmKakijun = hbm1;
    InvalidateRect(g_hMainWnd, &rc, FALSE);
    SetForegroundWindow(g_hMainWnd);

    if (!g_bPlaying)
        return 0;

    Sleep(200);
    PlaySoundW(MAKEINTRESOURCEW(1), g_hInstance, SND_ASYNC | SND_NODEFAULT | SND_RESOURCE);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); ++i)
    {
        switch (v[i].type)
        {
        case STROKE::WAIT:
            Sleep(500);

            if (!g_bPlaying)
                return 0;

            PlaySoundW(MAKEINTRESOURCEW(1), g_hInstance, SND_ASYNC | SND_NODEFAULT | SND_RESOURCE);
            break;

        case STROKE::DOT:
            {
                CDC hdc(g_hMainWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

                CRgn hRgn2(MyCreateRegion(v, g_rgn_files[ires++]));
                CombineRgn(hRgn5, hRgn5, hRgn2, RGN_OR);
                FillRgn(hdcMem, hRgn5, g_hbrRed);
                SelectObject(hdcMem, hbmOld);

                InvalidateRect(g_hMainWnd, &rc, TRUE);
                Sleep(50);
            }
            break;

        case STROKE::LINEAR:
            {
                CDC hdc(g_hMainWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v, g_rgn_files[ires++]));

                double cost1 = std::cos(v[i].angle0 * M_PI / 180);
                double sint1 = std::sin(v[i].angle0 * M_PI / 180);

                // NULLREGIONでない場所を探す。
#define LEN (KAKIJUN_CENTER_XY * 1414 / 1000) // 半径 * √2
                for (k = -LEN; k < LEN; k += 20)
                {
                    if (!g_bPlaying)
                        return 0;

                    apt[0].x = LONG(KAKIJUN_CENTER_XY + k * cost1 + LEN * sint1);
                    apt[0].y = LONG(KAKIJUN_CENTER_XY + k * sint1 - LEN * cost1);
                    apt[1].x = LONG(KAKIJUN_CENTER_XY + k * cost1 - LEN * sint1);
                    apt[1].y = LONG(KAKIJUN_CENTER_XY + k * sint1 + LEN * cost1);
                    apt[2].x = LONG(KAKIJUN_CENTER_XY + (k + 20) * cost1 - LEN * sint1);
                    apt[2].y = LONG(KAKIJUN_CENTER_XY + (k + 20) * sint1 + LEN * cost1);
                    apt[3].x = LONG(KAKIJUN_CENTER_XY + (k + 20) * cost1 + LEN * sint1);
                    apt[3].y = LONG(KAKIJUN_CENTER_XY + (k + 20) * sint1 - LEN * cost1);

                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 4);
                    EndPath(hdcMem);

                    CRgn hRgn3(::PathToRegion(hdcMem));
                    CRgn hRgn4(::CreateRectRgn(0, 0, 0, 0));
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    if (n != NULLREGION)
                        break;
                }

                // NULLREGIONでない位置から赤い画を描画する。
                for ( ; k < LEN; k += 20)
                {
                    if (!g_bPlaying)
                        return 0;

                    hbm1.Swap(hbm2);
                    g_hbmKakijun = hbm1;

                    hbmOld = SelectObject(hdcMem, hbm1);
                    apt[0].x = LONG(KAKIJUN_CENTER_XY + k * cost1 + LEN * sint1);
                    apt[0].y = LONG(KAKIJUN_CENTER_XY + k * sint1 - LEN * cost1);
                    apt[1].x = LONG(KAKIJUN_CENTER_XY + k * cost1 - LEN * sint1);
                    apt[1].y = LONG(KAKIJUN_CENTER_XY + k * sint1 + LEN * cost1);
                    apt[2].x = LONG(KAKIJUN_CENTER_XY + (k + 20) * cost1 - LEN * sint1);
                    apt[2].y = LONG(KAKIJUN_CENTER_XY + (k + 20) * sint1 + LEN * cost1);
                    apt[3].x = LONG(KAKIJUN_CENTER_XY + (k + 20) * cost1 + LEN * sint1);
                    apt[3].y = LONG(KAKIJUN_CENTER_XY + (k + 20) * sint1 - LEN * cost1);

                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 4);
                    EndPath(hdcMem);

                    CRgn hRgn3(::PathToRegion(hdcMem));
                    CRgn hRgn4(::CreateRectRgn(0, 0, 0, 0));
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    CombineRgn(hRgn5, hRgn5, hRgn4, RGN_OR);
                    FillRgn(hdcMem, hRgn5, g_hbrRed);

                    SelectObject(hdcMem, hbmOld);

                    InvalidateRect(g_hMainWnd, &rc, FALSE);
                    if (n == NULLREGION)
                        break;

                    Sleep(35);
                }
            }
            break;

        case STROKE::POLAR:
            {
                CDC hdc(g_hMainWnd);
                CDC hdcMem(hdc);

                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v, g_rgn_files[ires++]));

                INT step = 0;
                for (; step < KAKIJUN_SIZE / 20; ++step)
                {
                    CRgn hRgn8(::CreateRectRgn(0, 0, 0, 0));
                    CRgn hRgn9(::CreateEllipticRgn(v[i].cx - 20 * step, v[i].cy - 20 * step, v[i].cx + 20 * step, v[i].cy + 20 * step));
                    if (CombineRgn(hRgn8, hRgn2, hRgn9, RGN_AND) != NULLREGION)
                        break;
                };
                INT dk = 50 / (step + 2);

                BOOL found = FALSE;
                INT sign = (v[i].angle0 <= v[i].angle1) ? +1 : -1;
                INT k0 = v[i].angle0, k1 = v[i].angle1;
                for (k = k0; k * sign <= k1 * sign; k += dk * sign)
                {
                    if (!g_bPlaying)
                        return 0;

                    double theta = k * M_PI / 180.0;
                    double theta2 = (k + dk * sign) * M_PI / 180.0;
                    double cost1 = std::cos(theta);
                    double sint1 = std::sin(theta);
                    double cost2 = std::cos(theta2);
                    double sint2 = std::sin(theta2);
                    double cost3 = std::cos((2 * theta + 1 * theta2) / 3);
                    double sint3 = std::sin((2 * theta + 1 * theta2) / 3);
                    double cost4 = std::cos((1 * theta + 2 * theta2) / 3);
                    double sint4 = std::sin((1 * theta + 2 * theta2) / 3);

                    hbm1.Swap(hbm2);
                    g_hbmKakijun = hbm1;

                    hbmOld = SelectObject(hdcMem, hbm1);

                    apt[0].x = LONG(v[i].cx + 2 * LEN * cost1);
                    apt[0].y = LONG(v[i].cy + 2 * LEN * sint1);
                    apt[1].x = LONG(v[i].cx + 2 * LEN * cost3);
                    apt[1].y = LONG(v[i].cy + 2 * LEN * sint3);
                    apt[2].x = LONG(v[i].cx + 2 * LEN * cost4);
                    apt[2].y = LONG(v[i].cy + 2 * LEN * sint4);
                    apt[3].x = LONG(v[i].cx + 2 * LEN * cost2);
                    apt[3].y = LONG(v[i].cy + 2 * LEN * sint2);
                    apt[4].x = v[i].cx;
                    apt[4].y = v[i].cy;

                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 5);
                    EndPath(hdcMem);

                    CRgn hRgn3(::PathToRegion(hdcMem));
                    CRgn hRgn4(::CreateRectRgn(0, 0, 0, 0));
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    CombineRgn(hRgn5, hRgn5, hRgn4, RGN_OR);
                    FillRgn(hdcMem, hRgn5, g_hbrRed);

                    SelectObject(hdcMem, hbmOld);

                    InvalidateRect(g_hMainWnd, &rc, TRUE);

                    if (n == NULLREGION)
                    {
                        if (found)
                            break;
                    }
                    else
                    {
                        found = TRUE;
                    }

                    Sleep(35);
                }
            }
            break;
        }
    }

    Sleep(500);

    {
        CDC hdc(g_hMainWnd);
        CDC hdcMem(hdc);

        hbm1.Swap(hbm2);
        g_hbmKakijun = hbm1;

        hbmOld = SelectObject(hdcMem, hbm1);
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SelectObject(hdcMem, hbmOld);

        InvalidateRect(g_hMainWnd, &rc, FALSE);
    }

    Sleep(500);

    g_hbmKakijun = NULL;
    return 0;
}

unsigned __stdcall ThreadProc(void *) {
    ThreadProcWorker();
    g_bPlaying = FALSE;
    g_iFrame = -1;
    RECT rc = { 0, 0, 254, 254 };
    InvalidateRect(g_hMainWnd, &rc, TRUE);
    return 0;
}

void DoPlay(HWND hwnd) {
    g_bPlaying = TRUE;
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
    CloseHandle(hThread);
}

std::vector<STROKE> DoLoadKKJBinary(std::string& binary)
{
    std::vector<STROKE> v;
    STROKE stroke;
    std::vector<std::string> values;
    mstr_split(values, binary, ";");
    for (size_t i = 0; i < values.size(); ++i) {
        std::string& value = values[i];
        mstr_trim(value, " \t\r\n");
        std::vector<std::string> fields;
        mstr_split(fields, value, ",");
        std::string binary;
        switch (value[0]) {
        case 'W':
            stroke.type = STROKE::WAIT;
            v.push_back(stroke);
            break;
        case 'L':
            stroke.type = STROKE::LINEAR;
            stroke.angle0 = atoi(fields[1].c_str());
            v.push_back(stroke);
            break;
        case 'D':
            stroke.type = STROKE::DOT;
            v.push_back(stroke);
            break;
        case 'P':
            stroke.type = STROKE::POLAR;
            stroke.angle0 = atoi(fields[1].c_str());
            stroke.angle1 = atoi(fields[2].c_str());
            stroke.cx = atoi(fields[3].c_str());
            stroke.cy = atoi(fields[4].c_str());
            v.push_back(stroke);
            break;
        default:
            assert(0);
            break;
        }
    }
    return v;
}

BOOL DoLoadKKJ(LPCWSTR filename) {
    g_kkj.clear();

    if (lstrcmpiW(PathFindExtensionW(filename), L".kkj") != 0) {
        return FALSE;
    }

    std::string binary;
    if (!read_all(binary, filename)) {
        return FALSE;
    }

    g_kkj = DoLoadKKJBinary(binary);

    WCHAR path[MAX_PATH];
    lstrcpynW(path, filename, _countof(path));
    PathRemoveExtensionW(path);

    WCHAR path2[MAX_PATH];
    g_rgn_files.clear();
    for (INT i = 0; i < 100; ++i) {
        wnsprintfW(path2, _countof(path2), L"%s-%02d.rgn", path, i);

        if (!PathFileExistsW(path2))
            break;

        g_rgn_files.push_back(path2);
    }

    g_iFrame = -1;
    InvalidateRect(g_hMainWnd, NULL, TRUE);
    return TRUE;
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
    g_hMainWnd = hwnd;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    DragAcceptFiles(hwnd, TRUE);

    if (lParam) {
        DoLoadKKJ((LPWSTR)lParam);
    }
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
    switch (id) {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    case psh1: // Play
        g_iFrame = -1;
        DoPlay(hwnd);
        break;
    case psh2: // Stop
        g_iFrame = -1;
        g_bPlaying = FALSE;
        g_hbmKakijun = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case psh3: // >
        g_bPlaying = FALSE;
        Sleep(200);
        if (g_iFrame == -1)
            g_iFrame = 0;
        else if (g_iFrame + 1 < (INT)g_rgn_files.size())
            ++g_iFrame;
        g_hbmKakijun = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case psh4: // <
        g_bPlaying = FALSE;
        Sleep(200);
        if (g_iFrame == -1)
            g_iFrame = 0;
        else if (g_iFrame > 0)
            --g_iFrame;
        g_hbmKakijun = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case psh5:
        {
            WCHAR file[MAX_PATH] = L"";
            OPENFILENAMEW ofn = { OPENFILENAME_SIZE_VERSION_400W };
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Bitmap Image (*.bmp)\0*.bmp\0";
            ofn.lpstrFile = file;
            ofn.nMaxFile = _countof(file);
            ofn.lpstrTitle = L"Save as BMP";
            ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = L"bmp";
            if (GetSaveFileNameW(&ofn)) {
                HDC hdcMem = CreateCompatibleDC(NULL);
                HBITMAP hbm = CreateCompatibleBitmap(hdcMem, 254, 254);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hbm);
                RECT rc = { 0, 0, 254, 254 };
                OnDraw(hwnd, hdcMem, rc);
                SelectObject(hdcMem, hbmOld);

                SaveBitmapToFile(file, hbm);
                DeleteObject(hbm);
                DeleteDC(hdcMem);
            }
        }
    }
}

void OnPaint(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps)) {
        OnDraw(hwnd, hdc, rc);
        EndPaint(hwnd, &ps);
    }
}

void OnDestroy(HWND hwnd)
{
    DeleteObject(g_hbrRed);
    g_hMainWnd = NULL;
}

void OnDropFiles(HWND hwnd, HDROP hdrop)
{
    WCHAR file[MAX_PATH];
    DragQueryFileW(hdrop, 0, file, _countof(file));
    DoLoadKKJ(file);
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hwnd, WM_DROPFILES, OnDropFiles);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    }
    return FALSE;
}

INT Kakijun_Main(HINSTANCE hInstance, INT argc, wchar_t **wargv, INT nCmdShow) {
    g_hInstance = hInstance;
    InitCommonControls();
    DialogBoxParamW(hInstance, MAKEINTRESOURCEW(1), NULL, DialogProc, 
                    (LPARAM)((argc >= 2) ? wargv[1] : NULL));
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    INT argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    INT ret = Kakijun_Main(hInstance, argc, wargv, nCmdShow);
    LocalFree(wargv);
    return ret;
}
