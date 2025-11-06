// Moji No Benkyou (7)
// Copyright (C) 2025 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// License: MIT

// Detect memory leaks (Visual C++ only)
#if defined(_MSC_VER) && !defined(NDEBUG) && !defined(_CRTDBG_MAP_ALLOC)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <process.h>
#include <tchar.h>
#include <cstring>
#include <cassert>

#include <cstdlib>
#include <process.h>
#include <cmath>

#include <vector>
#include <string>
#include <map>
#include <set>

#include "kakijun.h"
#include "../CGdiObj.h"
#include "../CDebug.h"
#include "../Common.h"
#include "../FuriganaCtl/FuriganaCtl/furigana_api.h"
#include "../MyLib/MyLib.h"
#include "../mstr.h"

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

#define SLIDE_TIMER 999

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (7)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (7) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;
POINT g_ptDragging;

HBITMAP g_hbmClient = NULL;
HBITMAP g_hbmLeft = NULL;
HBITMAP g_hbmRight = NULL;

HBITMAP g_hbmKakijun; // Week ref
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;
INT g_iPage = 0;
float g_eGoalPage = 0;
float g_eDisplayPage = 0;

std::set<INT> g_kanji4_history;

BOOL g_bHighSpeed = FALSE;

extern "C" extern const wchar_t *g_aszMojiReadings[];
extern "C" extern const wchar_t *g_aszMojiExamples[];

std::wstring g_section;
MyLib *g_pMyLib = NULL;
MyLibStringTable *g_pMoji = NULL;
std::vector<HBITMAP> g_ahbmMoji;
KAKIJUN g_kakijun;
BOOL g_fJapanese = FALSE;

void EnumData() {
    WCHAR file[MAX_PATH];

    for (size_t i = 0; i < g_pMoji->size(); ++i) {
        std::wstring moji = g_pMoji->key_at(i);

#if 0
        DWORD size;
        PVOID pres = MyLoadRes(g_hInstance, L"GIF", MAKEINTRESOURCEW(1000 + i), &size);
        std::string binary((char *)pres, size);
        wsprintfW(file, L"%s\\i\\%s.gif", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(binary, file);
#endif

        // Load GIF
        wsprintfW(file, L"%s\\i\\%s.gif", g_section.c_str(), moji.c_str());
        HBITMAP hbm = g_pMyLib->load_picture(file);
        assert(hbm);
        g_ahbmMoji.push_back(hbm);

#if 0
        {
            DWORD size;
            PVOID pres = MyLoadRes(g_hInstance, L"MP3", MAKEINTRESOURCEW(1000 + i), &size);
            std::string binary((char *)pres, size);
            wsprintfW(file, L"%s\\s\\%s.mp3", g_section.c_str(), moji.c_str());
            g_pMyLib->save_binary(binary, file);
        }
#endif

#if 0
        std::vector<STROKE> v = g_kanji4_kakijun[i];
        std::vector<std::string> values;
        char buf[MAX_PATH];
        for (size_t i = 0; i < v.size(); ++i) {
            switch (v[i].type) {
            case STROKE::WAIT:
                values.push_back("W");
                break;
            case STROKE::LINEAR:
                wsprintfA(buf, "L,%d", v[i].angle0);
                values.push_back(buf);
                break;
            case STROKE::DOT:
                wsprintfA(buf, "D");
                values.push_back(buf);
                break;
            case STROKE::POLAR:
                wsprintfA(buf, "P,%d,%d,%d,%d", v[i].angle0, v[i].angle1, v[i].cx, v[i].cy);
                values.push_back(buf);
                break;
            }
        }
        std::string ansi = mstr_join(values, ";");
        wsprintfW(file, L"%s\\kkj\\%s.kkj", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(ansi, file);
#endif

#if 0
        {
            INT iKakijun = (100 + i) * 100;
            INT ires = 0;
            for (size_t k = 0; k < g_kanji4_kakijun[i].size(); ++k) {
                if (g_kanji4_kakijun[i][k].type != STROKE::WAIT) {
                    DWORD size;
                    PVOID pres = MyLoadRes(g_hInstance, RT_RCDATA, MAKEINTRESOURCEW(iKakijun + ires), &size);
                    std::string binary((char *)pres, size);
                    assert(size);
                    wsprintfW(file, L"%s\\kkj\\%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)ires);
                    g_pMyLib->save_binary(binary, file);
                    ++ires;
                }
            }
        }
#endif

        {
            std::vector<STROKE> v;
            STROKE stroke;
            wsprintfW(file, L"%s\\kkj\\%s.kkj", g_section.c_str(), moji.c_str());
            std::string ansi;
            g_pMyLib->load_binary(ansi, file);
            std::vector<std::string> values;
            mstr_split(values, ansi, ";");
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
            g_kakijun.push_back(v);
        }
    }
}

// WM_CREATE
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    // メディアライブラリを作成
    g_pMyLib = new(std::nothrow) MyLib();
    if (!g_pMyLib)
        return FALSE;

    // セクション名を読み込む
    g_section = LoadStringDx(500);
    assert(g_section.size());

    // 文字データを取り込む
    g_pMoji = new(std::nothrow) MyLibStringTable();
    if (!g_pMoji)
        return FALSE;
    g_pMyLib->load_string_table(*g_pMoji, g_section + L"\\Text.txt");

    // 開始時の音を鳴らす。これにより最初の音遅れを回避する。
    std::wstring start_sound = g_pMyLib->find_data_file(g_section + L"\\Start.mp3");
    g_pMyLib->play_sound_async(start_sound);

    EnumData();

    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    WCHAR file[MAX_PATH];

    wsprintfW(file, L"%s\\Left.gif", g_section.c_str());
    g_hbmLeft = g_pMyLib->load_picture(file);

    wsprintfW(file, L"%s\\Right.gif", g_section.c_str());
    g_hbmRight = g_pMyLib->load_picture(file);

    updateSystemMenu(hwnd);

    g_hbmClient = NULL;

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lstrcpyn(lf.lfFaceName, TEXT("ピザPゴシック"), _countof(lf.lfFaceName));
    lf.lfHeight = -35;
    lf.lfWeight = FW_BOLD;
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lf.lfQuality = PROOF_QUALITY;
    g_hFont = CreateFontIndirect(&lf);

    lf.lfHeight = -15;
    g_hFontSmall = CreateFontIndirect(&lf);

    INT cx = GetSystemMetrics(SM_CXBORDER);
    INT cy = GetSystemMetrics(SM_CYBORDER);
    g_hKakijunWnd = CreateWindow(g_szKakijunClassName, TEXT(""),
        WS_POPUPWINDOW, CW_USEDEFAULT, 0, KAKIJUN_SIZE + cx * 2, KAKIJUN_SIZE + cy * 2,
        hwnd, NULL, g_hInstance, NULL);
    if (g_hKakijunWnd == NULL)
        return FALSE;

    return TRUE;
}

// WM_DESTROY
void OnDestroy(HWND hwnd)
{
    if (g_hThread)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
        CloseHandle(g_hThread);
    }

    DeleteObject(g_hFont);
    DeleteObject(g_hFontSmall);

    UINT i;
    for (i = 0; i < (INT)g_ahbmMoji.size(); ++i)
    {
        if (g_ahbmMoji[i])
            DeleteObject(g_ahbmMoji[i]);
    }
    g_ahbmMoji.clear();

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmLeft);
    DeleteObject(g_hbmRight);

    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbrRed);

    g_kanji4_history.clear();

    g_kakijun.clear();

    delete g_pMoji;
    g_pMoji = NULL;

    delete g_pMyLib;
    g_pMyLib = NULL;

    PostQuitMessage(0);
}

#define COLUMNS 10
#define ROWS 7

VOID GetMojiRect(INT j, LPRECT prc)
{
    RECT rcC;
    GetClientRect(g_hMainWnd, &rcC);
    BITMAP bm;
    GetObjectW(g_ahbmMoji[0], sizeof(bm), &bm);

    INT iPage = j / (ROWS * COLUMNS);
    j %= (ROWS * COLUMNS);
    INT ix = j % COLUMNS;
    INT iy = j / COLUMNS;
    prc->left = ix * (bm.bmWidth + 20) + 5 + 25;
    prc->top = iy * (bm.bmHeight + 20) + 5 + 10;
    prc->right = prc->left + bm.bmWidth + 10;
    prc->bottom = prc->top + bm.bmHeight + 10;
    OffsetRect(prc, iPage * (bm.bmWidth + 20) * COLUMNS, 0);
    OffsetRect(prc, (LONG)(-g_eDisplayPage * (bm.bmHeight + 20) * COLUMNS), 0);
}

INT GetNumPage(VOID)
{
    return INT(g_ahbmMoji.size() + COLUMNS * ROWS - 1) / (COLUMNS * ROWS);
}

BOOL GetLeftArrowRect(HWND hwnd, LPRECT prc)
{
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    SetRect(prc, 0, rcClient.bottom - 48, 40, rcClient.bottom - 48 + 40);
    return (g_iPage > 0);
}

BOOL GetRightArrowRect(HWND hwnd, LPRECT prc)
{
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    SetRect(prc, rcClient.right - 40, rcClient.bottom - 48, rcClient.right, rcClient.bottom - 48 + 40);
    return g_iPage + 1 < GetNumPage();
}

VOID OnDraw(HWND hwnd, HDC hdc)
{
    HGDIOBJ hbmOld, hbmOld2;
    INT j;
    RECT rc;
    SIZE siz;
    HBRUSH hbr;

    CDC hdcMem(hdc);
    CDC hdcMem2(hdc);

    GetClientRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;
    if (g_hbmClient == NULL)
    {
        g_hbmClient = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
        hbr = CreateSolidBrush(RGB(255, 255, 192));
        FillRect(hdcMem2, &rc, hbr);
        DeleteObject(hbr);

        for (j = 0; j < (INT)g_ahbmMoji.size(); ++j)
        {
            GetMojiRect(j, &rc);
            hbmOld = SelectObject(hdcMem, g_ahbmMoji[j]);
            if (g_kanji4_history.find(j) != g_kanji4_history.end())
                FillRect(hdcMem2, &rc, g_hbrRed);
            else
                FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            InflateRect(&rc, -5, -5);
            BitBlt(hdcMem2, rc.left, rc.top, 40, 40, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        // Left arrow
        if (GetLeftArrowRect(hwnd, &rc))
        {
            hbmOld = SelectObject(hdcMem, g_hbmLeft);
            BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        // Right arrow
        if (GetRightArrowRect(hwnd, &rc))
        {
            hbmOld = SelectObject(hdcMem, g_hbmRight);
            BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        GetClientRect(hwnd, &rc);
        rc.top = rc.bottom - 48;
        TCHAR szText[64];
        wsprintf(szText, TEXT("%u / %u"), (g_iPage + 1), GetNumPage());
        SetBkMode(hdcMem2, TRANSPARENT);
        DrawText(hdcMem2, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        SelectObject(hdcMem2, hbmOld2);
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
}

// WM_PAINT
void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        OnDraw(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
}

HRGN MyCreateRegion(INT nIndex, INT iKakijun, INT i, INT ires) {
#if 1
    std::vector<STROKE>& v = g_kakijun[nIndex];
    std::wstring moji = g_pMoji->key_at(nIndex);
    INT k = ires;
    assert(v[i].type != STROKE::WAIT);
    WCHAR file[MAX_PATH];
    wsprintfW(file, L"%s\\kkj\\%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)k);
    std::string binary;
    g_pMyLib->load_binary(binary, file);
    return DeserializeRegion254((PBYTE)binary.c_str(), (DWORD)binary.size());
#else
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(res), RT_RCDATA);
    DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    return DeserializeRegion254((PBYTE)pvData, cbData);
#endif
}

void GetStrokeData(std::vector<STROKE>& v)
{
    v = g_kakijun[g_nMoji];
}

void PreDraw(HDC hdc, RECT& rc)
{
    // reserved
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    INT k;
    POINT apt[5];

    std::vector<STROKE> v;
    GetStrokeData(v);

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    INT nIndex = g_nMoji;
    INT iKakijun = (100 + nIndex) * 100;
    INT ires = 0;
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].type != STROKE::WAIT)
        {
            CRgn hRgn2(MyCreateRegion(nIndex, iKakijun, i, ires++));
            CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
        }
    }
    ires = 0;

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);
        hbm1 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbm2 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);

        hbmOld = SelectObject(hdcMem, hbm1);
        rc.left = 0;
        rc.top = 0;
        rc.right = siz.cx;
        rc.bottom = siz.cy;
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        PreDraw(hdcMem, rc);
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SelectObject(hdcMem, hbmOld);
    }

    g_hbmKakijun = hbm1;
    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);
    SetForegroundWindow(g_hKakijunWnd);

    std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\s\\" + g_pMoji->key_at(g_nMoji) + L".mp3");
    g_pMyLib->play_sound(mp3_path);

    if (!IsWindowVisible(g_hKakijunWnd))
        return 0;

    DO_SLEEP(200);

    std::wstring stroke_path = g_pMyLib->find_data_file(g_section + L"\\Stroke.mp3");
    g_pMyLib->play_sound_async(stroke_path);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); ++i)
    {
        switch (v[i].type)
        {
        case STROKE::WAIT:
            DO_SLEEP(500);

            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;

            g_pMyLib->play_sound_async(stroke_path);
            break;

        case STROKE::DOT:
#if 0 // Optimized
            {
                CDC hdc(g_hKakijunWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                PreDraw(hdcMem, rc);
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

                CRgn hRgn2(MyCreateRegion(nIndex, iKakijun, i, ires++));
                CombineRgn(hRgn5, hRgn5, hRgn2, RGN_OR);
                FillRgn(hdcMem, hRgn5, g_hbrRed);
                SelectObject(hdcMem, hbmOld);

                InvalidateRect(g_hKakijunWnd, NULL, TRUE);
                DO_SLEEP(50);
            }
#endif
            break;

        case STROKE::LINEAR:
            {
                CDC hdc(g_hKakijunWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                PreDraw(hdcMem, rc);
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(nIndex, iKakijun, i, ires++));

                double cost1 = std::cos(v[i].angle0 * M_PI / 180);
                double sint1 = std::sin(v[i].angle0 * M_PI / 180);

                // NULLREGIONでない場所を探す。
#define LEN (KAKIJUN_CENTER_XY * 1414 / 1000) // 半径 * √2
                for (k = -LEN; k < LEN; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
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
                    if (!IsWindowVisible(g_hKakijunWnd))
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

                    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
                    if (n == NULLREGION)
                        break;

                    DO_SLEEP(35);
                }
            }
            break;

        case STROKE::POLAR:
            {
                CDC hdc(g_hKakijunWnd);
                CDC hdcMem(hdc);

                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;

                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                PreDraw(hdcMem, rc);
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(nIndex, iKakijun, i, ires++));

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
                    if (!IsWindowVisible(g_hKakijunWnd))
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

                    InvalidateRect(g_hKakijunWnd, NULL, TRUE);

                    if (n == NULLREGION)
                    {
                        if (found)
                            break;
                    }
                    else
                    {
                        found = TRUE;
                    }

                    DO_SLEEP(35);
                }
            }
            break;
        }
    }

    DO_SLEEP(500);
    g_pMyLib->play_sound_async(mp3_path);

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);

        hbm1.Swap(hbm2);
        g_hbmKakijun = hbm1;

        hbmOld = SelectObject(hdcMem, hbm1);
        rc.left = rc.top = 0;
        rc.right = siz.cx;
        rc.bottom = siz.cy;
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        PreDraw(hdcMem, rc);
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SelectObject(hdcMem, hbmOld);

        InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    }

    DO_SLEEP(500);

    ShowWindow(g_hKakijunWnd, SW_HIDE);
    g_hbmKakijun = NULL;
    return 0;
}

unsigned __stdcall ThreadProc(void *)
{
    // Detect handle leaks
    OBJECTS_CHECK_POINT();

    ThreadProcWorker();

    // Detect handle leaks
    OBJECTS_CHECK_POINT();

    return 0;
}

VOID MojiOnClick(HWND hwnd, INT nMoji, BOOL fRight)
{
    RECT rc, rc2;
    g_nMoji = nMoji;

    if (fRight)
    {
        SetForegroundWindow(hwnd);
        HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(100));
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        POINT pt;
        GetCursorPos(&pt);
        INT nCmd = TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
        if (nCmd)
            PostMessage(hwnd, WM_COMMAND, nCmd, 0);
        return;
    }

    GetWindowRect(hwnd, &rc);
    GetWindowRect(g_hKakijunWnd, &rc2);
    MoveWindow(g_hKakijunWnd,
        rc.left + (rc.right - rc.left - (rc2.right - rc2.left)) / 2,
        rc.top + (rc.bottom - rc.top - (rc2.bottom - rc2.top)) / 2,
        rc2.right - rc2.left,
        rc2.bottom - rc2.top,
        TRUE);

    // 読み。
    LPCWSTR psz = g_aszMojiReadings[g_nMoji];
    LPCWSTR pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption1, (pch + 1));

    // 意味。
    WCHAR szMeaning[128];
    LoadString(g_hInstance, 2000 + g_nMoji, szMeaning, _countof(szMeaning));
    psz = szMeaning;
    pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption2, (pch + 1));

    g_kanji4_history.insert(nMoji);

    if (g_hbmClient)
        DeleteObject(g_hbmClient);
    g_hbmClient = NULL;
    InvalidateRect(hwnd, NULL, FALSE);

    if (g_hThread)
        CloseHandle(g_hThread);
    g_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
}

VOID OnButtonDown(HWND hwnd, INT x, INT y, BOOL fRight)
{
    INT j;
    POINT pt;
    RECT rc;
    SIZE siz;
    DWORD dw;

    GetClientRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    dw = 0;
    GetExitCodeThread(g_hThread, &dw);
    if (dw == STILL_ACTIVE)
        return;

    pt.x = x;
    pt.y = y;

    if (GetLeftArrowRect(hwnd, &rc))
    {
        if (PtInRect(&rc, pt))
        {
            std::wstring slide_path = g_pMyLib->find_data_file(g_section + L"\\Slide.mp3");
            g_pMyLib->play_sound_async(slide_path);

            g_eDisplayPage = (float)g_iPage;
            g_eGoalPage = (float)(g_iPage - 1);
            SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
            return;
        }
    }

    if (GetRightArrowRect(hwnd, &rc))
    {
        if (PtInRect(&rc, pt))
        {
            std::wstring slide_path = g_pMyLib->find_data_file(g_section + L"\\Slide.mp3");
            g_pMyLib->play_sound_async(slide_path);

            g_eDisplayPage = (float)g_iPage;
            g_eGoalPage = (float)(g_iPage + 1);
            SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
            return;
        }
    }

    for (j = 0; j < (INT)g_ahbmMoji.size(); ++j)
    {
        GetMojiRect(j, &rc);
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, j, fRight);
            return;
        }
    }

    GetCursorPos(&g_ptDragging);
    SetCapture(hwnd);
}

// WM_SETCURSOR
BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
    if (codeHitTest != HTCLIENT)
        return FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, msg, DefWindowProc);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);

    RECT rc;

    if (GetLeftArrowRect(hwnd, &rc) && PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    if (GetRightArrowRect(hwnd, &rc) && PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    for (UINT j = 0; j < g_ahbmMoji.size(); ++j)
    {
        GetMojiRect(j, &rc);
        if (PtInRect(&rc, pt))
        {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return TRUE;
}

VOID Kakijun_OnDraw(HWND hwnd, HDC hdc)
{
    RECT rc;
    SIZE siz;
    HGDIOBJ hbmOld;

    GetClientRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;
    if (g_hbmKakijun)
    {
        CDC hdcMem(hdc);
        hbmOld = SelectObject(hdcMem, g_hbmKakijun);
        BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
    }
}

// WM_CREATE
BOOL Kakijun_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    // キャプションウィンドウの作成
    DWORD style = WS_POPUP | WS_BORDER | ES_CENTER | ES_MULTILINE | ES_AUTOVSCROLL | FCS_NOSCROLL;
    DWORD exstyle = WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    g_hwndCaption1 = CreateWindowEx(exstyle, L"FuriganaCtl", NULL,
        style, 0, 0, 550, 400,
        GetParent(hwnd), NULL, g_hInstance, NULL);
    g_hwndCaption2 = CreateWindowEx(exstyle, L"FuriganaCtl", NULL,
        style, 0, 0, 550, 400,
        GetParent(hwnd), NULL, g_hInstance, NULL);

    // キャプションウィンドウの設定
    ::SendMessageW(g_hwndCaption1, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    ::SendMessageW(g_hwndCaption2, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    ::SendMessageW(g_hwndCaption1, FC_SETRUBYRATIO, 3, 5);
    ::SendMessageW(g_hwndCaption2, FC_SETRUBYRATIO, 3, 5);

    RECT rc = { 6, 6, 6, 6 };
    ::SendMessageW(g_hwndCaption1, FC_SETMARGIN, 0, (LPARAM)&rc);
    ::SendMessageW(g_hwndCaption2, FC_SETMARGIN, 0, (LPARAM)&rc);

    ::SendMessageW(g_hwndCaption1, FC_SETCOLOR, 0, RGB(0, 0, 0));
    ::SendMessageW(g_hwndCaption2, FC_SETCOLOR, 0, RGB(0, 0, 0));
    ::SendMessageW(g_hwndCaption1, FC_SETCOLOR, 1, RGB(255, 255, 130));
    ::SendMessageW(g_hwndCaption2, FC_SETCOLOR, 1, RGB(255, 255, 130));

    ::SendMessageW(g_hwndCaption1, FC_SETLINEGAP, 6, 0);
    ::SendMessageW(g_hwndCaption2, FC_SETLINEGAP, 6, 0);

    return TRUE;
}

void MoveCaptionWnd(HWND hwnd, HWND hwndCaption, INT nIndex)
{
    RECT rc;
    ::GetWindowRect(hwnd, &rc);

    ::SendMessageW(hwndCaption, FC_SETSEL, -1, 0);
    ::SendMessageW(hwndCaption, WM_HSCROLL, SB_LEFT, 0);
    ::SendMessageW(hwndCaption, WM_VSCROLL, SB_TOP, 0);

    DWORD style = (DWORD)GetWindowLongPtrW(hwndCaption, GWL_STYLE);
    style &= ~(WS_HSCROLL | WS_VSCROLL);
    SetWindowLongPtrW(hwndCaption, GWL_STYLE, style);

    RECT rcIdeal = { 0, 0, 550, 50 };
    ::SendMessageW(hwndCaption, FC_GETIDEALSIZE, 0, (LPARAM)&rcIdeal);
    SIZE siz = { rcIdeal.right - rcIdeal.left, rcIdeal.bottom - rcIdeal.top };

    RECT rcNew;
    rcNew.left = (rc.left + rc.right - siz.cx) / 2;
    rcNew.top = (nIndex == 0) ? (rc.top - 10 - siz.cy) : (rc.bottom + 10);
    rcNew.right = rcNew.left + siz.cx;
    rcNew.bottom = rcNew.top + siz.cy;

    ::MoveWindow(hwndCaption, rcNew.left, rcNew.top, rcNew.right - rcNew.left, rcNew.bottom - rcNew.top, FALSE);

    ::InvalidateRect(hwndCaption, NULL, TRUE);
}

// WM_SHOWWINDOW
void Kakijun_OnShowWindow(HWND hwnd, BOOL fShow, UINT status)
{
    if (fShow)
    {
        MoveCaptionWnd(hwnd, g_hwndCaption1, 0);
        MoveCaptionWnd(hwnd, g_hwndCaption2, 1);
        ShowWindow(g_hwndCaption1, SW_SHOWNOACTIVATE);
        ShowWindow(g_hwndCaption2, SW_SHOWNOACTIVATE);
    }
    else
    {
        ShowWindow(g_hwndCaption1, SW_HIDE);
        ShowWindow(g_hwndCaption2, SW_HIDE);
    }
}

// WM_ERASEBKGND
inline BOOL Kakijun_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

// WM_PAINT
void Kakijun_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        Kakijun_OnDraw(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
}

// WM_DESTROY
void Kakijun_OnDestroy(HWND hwnd)
{
    DestroyWindow(g_hwndCaption1);
    g_hwndCaption1 = NULL;
    DestroyWindow(g_hwndCaption2);
    g_hwndCaption2 = NULL;
}

// WM_RBUTTONDOWN
void Kakijun_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    ShowWindow(hwnd, SW_HIDE);
}

// WM_KEYDOWN
void Kakijun_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (!fDown)
        return;
    if (vk == VK_ESCAPE)
        ShowWindow(hwnd, SW_HIDE);
}

// WM_NOTIFY
LRESULT Kakijun_OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    FURIGANA_NOTIFY *notify = (FURIGANA_NOTIFY *)pnmhdr;
    switch (pnmhdr->code) {
    case NM_KEYDOWN:
        if (notify->vk == VK_ESCAPE) {
            ShowWindow(hwnd, SW_HIDE);
            return TRUE;
        }
    }
    return FALSE;
}

LRESULT CALLBACK
KakijunWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, Kakijun_OnCreate);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, Kakijun_OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, Kakijun_OnPaint);
        HANDLE_MSG(hwnd, WM_SHOWWINDOW, Kakijun_OnShowWindow);
        HANDLE_MSG(hwnd, WM_DESTROY, Kakijun_OnDestroy);
        HANDLE_MSG(hwnd, WM_KEYDOWN, Kakijun_OnKey);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Kakijun_OnRButtonDown);
        HANDLE_MSG(hwnd, WM_NOTIFY, Kakijun_OnNotify);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// WM_ERASEBKGND
BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}

// WM_LBUTTONDOWN
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (fDoubleClick)
        return;

    OnButtonDown(hwnd, x, y, FALSE);
}

// WM_RBUTTONDOWN
void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (fDoubleClick)
        return;

    OnButtonDown(hwnd, x, y, TRUE);
}

// WM_SYSCOMMAND
void OnSysCommand(HWND hwnd, UINT cmd, int x, int y)
{
    if (GET_SC_WPARAM(cmd) == SYSCOMMAND_ABOUT)
    {
        DialogBox(g_hInstance, MAKEINTRESOURCE(1), hwnd, AboutDialogProc);
        return;
    }

    if (GET_SC_WPARAM(cmd) == SYSCOMMAND_HIGH_SPEEED)
    {
        g_bHighSpeed = !g_bHighSpeed;
        HMENU hSysMenu = ::GetSystemMenu(hwnd, FALSE);
        ::CheckMenuItem(hSysMenu, SYSCOMMAND_HIGH_SPEEED, (g_bHighSpeed ? MF_CHECKED : MF_UNCHECKED));
        return;
    }

    if (GET_SC_WPARAM(cmd) == SYSCOMMAND_STUDY_USING_ENGLISH)
    {
        rememberStudyMode(hwnd, STUDY_MODE_USING_ENGLISH);
        return;
    }

    if (GET_SC_WPARAM(cmd) == SYSCOMMAND_STUDY_USING_JAPANESE)
    {
        rememberStudyMode(hwnd, STUDY_MODE_USING_JAPANESE);
        return;
    }

    FORWARD_WM_SYSCOMMAND(hwnd, cmd, x, y, DefWindowProc);
}

#include "../KanjiDataDlg.h"

// WM_COMMAND
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    TCHAR szText[MAX_PATH], szURL[MAX_PATH];
    tstring str;

    g_kanji4_history.insert(g_nMoji);
    if (g_hbmClient)
    {
        DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
    }
    InvalidateRect(hwnd, NULL, TRUE);

    switch (id)
    {
    case 1000:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPWSTR pch = _tcschr(szText, TEXT(':'));
            *pch = 0;
            str = szText;
            wsprintf(szURL, LoadStringDx(1000), str.c_str());
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    case 1001:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPWSTR pch = _tcschr(szText, TEXT(':'));
            *pch = 0;
            wsprintf(szURL, LoadStringDx(1001), szText[0]);
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    case 1002:
    case 1003:
    case 1004:
    case 1007:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR pch = _tcschr(szText, TEXT(':'));
            *pch = 0;
            wsprintf(szURL, LoadStringDx(id), szText);
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    case 1006:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR pch = _tcschr(szText, TEXT(':'));
            *pch = 0;
            CopyText(hwnd, szText);
        }
        break;
    case 1010: // 漢字データ
        {
            KanjiDataDlg dlg;

            // 漢字
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR pch = _tcschr(szText, TEXT(':'));
            *pch = 0;
            dlg.m_text = szText;

            // 読み
            dlg.m_reading = _tcschr(g_aszMojiReadings[g_nMoji], L':') + 1;

            // 意味。
            LoadString(g_hInstance, 2000 + g_nMoji, szText, _countof(szText));
            dlg.m_meaning = _tcschr(szText, TEXT(':')) + 1;

            // 使い方
            dlg.m_examples = _tcschr(g_aszMojiExamples[g_nMoji], L':') + 1;

            // 「漢字データ」ダイアログを開く
            dlg.dialog_box(g_hInstance, hwnd);
        }
        break;
    }
}

// WM_TIMER
void OnTimer(HWND hwnd, UINT id)
{
    if (id == SLIDE_TIMER)
    {
        if (g_eDisplayPage < g_eGoalPage)
        {
            g_eDisplayPage += 0.35f;

            if (g_eDisplayPage > g_eGoalPage)
                g_eDisplayPage = g_eGoalPage;
        }
        else if (g_eDisplayPage > g_eGoalPage)
        {
            g_eDisplayPage -= 0.35f;

            if (g_eDisplayPage < g_eGoalPage)
                g_eDisplayPage = g_eGoalPage;
        }

        DeleteObject(g_hbmClient);
        g_hbmClient = NULL;

        InvalidateRect(hwnd, NULL, TRUE);

        if (g_eDisplayPage == g_eGoalPage)
        {
            g_iPage = (INT)g_eGoalPage;
            KillTimer(hwnd, id);
        }
    }
}

// WM_KEYDOWN
void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (!fDown)
        return;

    if (vk == VK_ESCAPE)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
    }
}

// WM_MOUSEMOVE
void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (hwnd != GetCapture())
        return;

    RECT rcClient;
    GetClientRect(hwnd, &rcClient);

    POINT pt;
    GetCursorPos(&pt);

    float eDelta = float(g_ptDragging.x - pt.x) / (rcClient.right / 4);
    if (eDelta < -1)
        eDelta = -1;
    else if (eDelta > +1)
        eDelta = +1;

    if (eDelta == -1 && g_iPage - 1 >= 0)
    {
        std::wstring slide_path = g_pMyLib->find_data_file(g_section + L"\\Slide.mp3");
        g_pMyLib->play_sound_async(slide_path);

        g_eDisplayPage = (float)g_iPage;
        g_eGoalPage = (float)(g_iPage - 1);
        SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
        return;
    }

    if (eDelta == +1 && g_iPage + 1 < GetNumPage())
    {
        std::wstring slide_path = g_pMyLib->find_data_file(g_section + L"\\Slide.mp3");
        g_pMyLib->play_sound_async(slide_path);

        g_eDisplayPage = (float)g_iPage;
        g_eGoalPage = (float)(g_iPage + 1);
        SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
        return;
    }
}

// WM_CANCELMODE
inline void OnCancelMode(HWND hwnd)
{
    ReleaseCapture();
}

// WM_LBUTTONUP
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    ReleaseCapture();
}

// WM_RBUTTONUP
void OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
    ReleaseCapture();
}

// WM_NOTIFY
LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    HMENU hMenu;
    FURIGANA_NOTIFY *notify = (FURIGANA_NOTIFY *)pnmhdr;
    if (idFrom != edt3 && idFrom != edt4)
        return FALSE;

    switch (pnmhdr->code)
    {
    case FCN_LOADCONTEXTMENU:
        hMenu = ::LoadMenuW(g_hInstance, MAKEINTRESOURCEW(101));
        if (hMenu)
        {
            WCHAR text[512];
            if (::SendMessageW(pnmhdr->hwndFrom, FC_GETSELTEXT, _countof(text), (LPARAM)text)) {
                BOOL bNoText = (text[0] == 0);
                ::EnableMenuItem(hMenu, 2000, bNoText ? MF_GRAYED : MF_ENABLED);
                ::EnableMenuItem(hMenu, 2001, bNoText ? MF_GRAYED : MF_ENABLED);
                ::EnableMenuItem(hMenu, 2003, bNoText ? MF_GRAYED : MF_ENABLED);
                ::EnableMenuItem(hMenu, 2004, bNoText ? MF_GRAYED : MF_ENABLED);
                ::EnableMenuItem(hMenu, 2005, bNoText ? MF_GRAYED : MF_ENABLED);
            }
        }
        return (LRESULT)hMenu;
    case FCN_CONTEXTMENUACTION:
        switch (notify->action_id) {
        case 2000: // コピー
            PostMessageW(pnmhdr->hwndFrom, WM_COPY, 0, 0);
            break;
        case 2001: // コピー (フリガナ付き)
            PostMessageW(pnmhdr->hwndFrom, WM_COPY, 1, 0);
            break;
        case 2002: // すべて選択
            PostMessageW(pnmhdr->hwndFrom, FC_SETSEL, 0, -1);
            break;
        case 2003: // Google検索
        case 2004: // Jisho.org
        case 2005: // Yahoo! JAPAN search
            {
                WCHAR text[512];
                if (::SendMessageW(pnmhdr->hwndFrom, FC_GETSELTEXT, _countof(text), (LPARAM)text)) {
                    WCHAR szURL[512];
                    switch (notify->action_id) {
                    case 2003:
                        wsprintfW(szURL, LoadStringDx(1005), text);
                        break;
                    case 2004:
                        wsprintfW(szURL, LoadStringDx(1006), text);
                        break;
                    case 2005:
                        wsprintfW(szURL, LoadStringDx(1007), text);
                        break;
                    default:
                        assert(0);
                        break;
                    }
                    ShellExecuteW(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
                }
            }
            break;
        }
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

// ウィンドウプロシージャ。
LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDBLCLK, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLButtonUp);
        HANDLE_MSG(hwnd, WM_RBUTTONUP, OnRButtonUp);
        HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMouseMove);
        HANDLE_MSG(hwnd, WM_CANCELMODE, OnCancelMode);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SYSCOMMAND, OnSysCommand);
        HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKey);
        HANDLE_MSG(hwnd, WM_SETCURSOR, OnSetCursor);
        HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

INT WINAPI WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       pszCmdLine,
    INT         nCmdShow)
{
    g_hInstance = hInstance;

    // レジストリから読み込んだスタディモードを適用。
    applyStudyMode(getStudyMode());

    // 日本語か？
    g_fJapanese = (PRIMARYLANGID(WonGetThreadUILanguage()) == LANG_JAPANESE);

    // コモンコントロール初期化。
    InitCommonControls();

    // 画面が小さすぎる場合はエラーメッセージを表示して終了。
    RECT rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
    INT cxWork = (rcWorkArea.right - rcWorkArea.left);
    INT cyWork = (rcWorkArea.bottom - rcWorkArea.top);
    if (cxWork <= 780 || cyWork <= 430)
    {
        MessageBox(NULL, LoadStringDx(4), NULL, MB_ICONERROR);
        return 1;
    }

    // フリガナコントロールを読み込む。
    HINSTANCE hinstDll = LoadLibraryW(L"FuriganaCtl.dll");
    if (!hinstDll) {
        MessageBox(NULL, LoadStringDx(6), NULL, MB_ICONERROR);
        return 2;
    }

    // ウィンドウクラスを登録する。
    WNDCLASSEX wcx = { sizeof(wcx) };
    wcx.style           = CS_DBLCLKS;
    wcx.lpfnWndProc     = WindowProc;
    wcx.hInstance       = hInstance;
    wcx.hIcon           = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wcx.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground   = (HBRUSH)CreateSolidBrush(RGB(255, 255, 192));
    wcx.lpszClassName   = g_szClassName;
    wcx.hIconSm         = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (!RegisterClassEx(&wcx))
        return 1;
    wcx.style           = CS_NOCLOSE;
    wcx.lpfnWndProc     = KakijunWndProc;
    wcx.hIcon           = NULL;
    wcx.hbrBackground   = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcx.lpszClassName   = g_szKakijunClassName;
    wcx.hIconSm         = NULL;
    if (!RegisterClassEx(&wcx))
        return 1;

    // クライアント領域のサイズとスタイルを元にウィンドウサイズを決める。
    DWORD style = WS_SYSMENU | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX;
    DWORD exstyle = 0;
    RECT rc = { 0, 0, 654, 496 };
    AdjustWindowRectEx(&rc, style, FALSE, exstyle);

    // ウィンドウサイズに基づいてメインウィンドウを作成する。
    g_hMainWnd = CreateWindowEx(exstyle, g_szClassName, LoadStringDx(1), style,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInstance, NULL);
    if (g_hMainWnd == NULL)
    {
        MessageBox(NULL, LoadStringDx(3), NULL, MB_ICONERROR);
        return 2;
    }

    // ウィンドウを表示する。
    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    // メッセージループ。
    MSG msg;
    BOOL f;
    while ((f = GetMessage(&msg, NULL, 0, 0)) != FALSE)
    {
        if (f == -1)
            return -1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    FreeLibrary(hinstDll);

    // Detect handle leaks
    OBJECTS_CHECK_POINT();

    // Detect memory leaks
#if defined(_MSC_VER) && !defined(NDEBUG) // Visual C++ only
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return (INT)msg.wParam;
}
