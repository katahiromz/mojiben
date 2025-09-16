// Moji No Benkyou (1)
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

#include <cstdlib>
#include <process.h>
#include <cmath>

#include <vector>
#include <map>
#include <set>
#include <cassert>

#include "kakijun.h"
#include "../CGdiObj.h"
#include "../CDebug.h"
#include "../Common.h"

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (1)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (1) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

#define NUM_MOJI 92

HBITMAP g_hbmHiragana, g_hbmHiragana2;
HBITMAP g_hbmKatakana, g_hbmKatakana2;
HBITMAP g_ahbmMoji[NUM_MOJI];
HBITMAP g_hbmClient;
BOOL g_fKatakana;

HBITMAP g_hbmKakijun = NULL;
INT g_nMoji;
HANDLE g_hThread = NULL;
HBRUSH g_hbrRed = NULL;

std::set<INT> g_katakana_history;
std::set<INT> g_hiragana_history;

BOOL g_bHighSpeed = FALSE;

struct MOJI
{
    SHORT index;
    SHORT moji_id;
    WCHAR wch;
    SHORT bitmap_id;
    SHORT x;
    SHORT y;
    BOOLEAN is_katakana;
    const char *romaji;
};

MOJI g_moji_data[NUM_MOJI] = {
#define DEFINE_MOJI(index, moji_id, wch, bitmap_id, x, y, is_katakana, romaji) \
    { index, moji_id, wch, bitmap_id, x, y, is_katakana, romaji },
#include "mojidata.h"
#undef DEFINE_MOJI
};

INT MojiIndexFromMojiID(INT moji_id)
{
    for (size_t i = 0; i < _countof(g_moji_data); ++i)
    {
        if (g_moji_data[i].moji_id == moji_id)
            return (INT)i;
    }
    assert(0);
    return -1;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));
    g_hbmHiragana = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100));
    g_hbmHiragana2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(150));
    g_hbmKatakana = LoadBitmap(g_hInstance, MAKEINTRESOURCE(200));
    g_hbmKatakana2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(250));
    g_fKatakana = FALSE;

    updateSystemMenu(hwnd);

    for (UINT i = 0; i < _countof(g_ahbmMoji); ++i)
    {
        assert(g_moji_data[i].index == i);
        g_ahbmMoji[i] = LoadGif(g_hInstance, g_moji_data[i].bitmap_id);
    }

    g_hbmClient = NULL;

    try
    {
        InitHiragana();
        InitKatakana();
    }
    catch (std::bad_alloc)
    {
        return FALSE;
    }

    INT cx = GetSystemMetrics(SM_CXBORDER);
    INT cy = GetSystemMetrics(SM_CYBORDER);
    g_hKakijunWnd = CreateWindow(g_szKakijunClassName, TEXT(""),
        WS_POPUPWINDOW, CW_USEDEFAULT, 0, 300 + cx * 2, 300 + cy * 2,
        hwnd, NULL, g_hInstance, NULL);
    if (g_hKakijunWnd == NULL)
        return FALSE;

    return TRUE;
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

        if (g_fKatakana)
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiragana2);
            BitBlt(hdcMem2, 160, 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakana);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        else
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiragana);
            BitBlt(hdcMem2, 160, 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakana2);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        for (j = 0; j < _countof(g_ahbmMoji); ++j)
        {
            assert(g_moji_data[j].index == j);
            if (g_moji_data[j].is_katakana != g_fKatakana)
                continue;

            hbmOld = SelectObject(hdcMem, g_ahbmMoji[j]);
            rc.left = g_moji_data[j].x;
            rc.top = g_moji_data[j].y;
            rc.right = rc.left + 50;
            rc.bottom = rc.top + 50;
            InflateRect(&rc, +3, +3);
            if (g_fKatakana)
            {
                if (g_katakana_history.find(g_moji_data[j].moji_id) != g_katakana_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else
            {
                if (g_hiragana_history.find(g_moji_data[j].moji_id) != g_hiragana_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            InflateRect(&rc, -3, -3);
            BitBlt(hdcMem2, rc.left, rc.top, 50, 50, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
}

HRGN MyCreateRegion(INT res)
{
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(res), RT_RCDATA);
    DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    return DeserializeRegion((const WORD *)pvData, cbData / sizeof(WORD));
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    std::vector<STROKE> v;
    INT k;
    POINT apt[5];
    const char *romaji = NULL;
    HGDIOBJ hFontOld;
    LOGFONT lf;

    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -20;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfQuality = ANTIALIASED_QUALITY;
    lstrcpy(lf.lfFaceName, TEXT("Tahoma"));
    CFont hFont(::CreateFontIndirect(&lf));

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    INT index = MojiIndexFromMojiID(g_nMoji);

    if (g_fKatakana)
        v = g_katakana_kakijun[index];
    else
        v = g_hiragana_kakijun[index];

    romaji = g_moji_data[index].romaji;

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].type != WAIT)
        {
            CRgn hRgn2(MyCreateRegion(v[i].res));
            CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
        }
    }

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
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        {
            hFontOld = SelectObject(hdcMem, hFont);
            SetTextColor(hdcMem, RGB(0, 0, 0));
            SetBkColor(hdcMem, RGB(255, 255, 255));
            SetBkMode(hdcMem, OPAQUE);
            DrawTextA(hdcMem, romaji, lstrlenA(romaji), &rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
            SelectObject(hdcMem, hFontOld);
        }
        SelectObject(hdcMem, hbmOld);
    }

    g_hbmKakijun = hbm1;
    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);

    MyPlaySound(MAKEINTRESOURCE(3000 + g_nMoji));
    if (!IsWindowVisible(g_hKakijunWnd))
        return 0;
    DO_SLEEP(200);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    MyPlaySoundAsync(MAKEINTRESOURCE(400));
    for (UINT i = 0; i < v.size(); i++)
    {
        switch (v[i].type)
        {
        case WAIT:
            DO_SLEEP(500);
            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;
            MyPlaySoundAsync(MAKEINTRESOURCE(400));
            break;

        case LINEAR:
            {
                CDC hdc(g_hKakijunWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;
                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = 0;
                rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                {
                    hFontOld = SelectObject(hdcMem, hFont);
                    SetTextColor(hdcMem, RGB(0, 0, 0));
                    SetBkColor(hdcMem, RGB(255, 255, 255));
                    SetBkMode(hdcMem, OPAQUE);
                    DrawTextA(hdcMem, romaji, lstrlenA(romaji), &rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
                    SelectObject(hdcMem, hFontOld);
                }
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v[i].res));
                double cost1 = cos(v[i].angle0 * M_PI / 180);
                double sint1 = sin(v[i].angle0 * M_PI / 180);
#define LEN (KAKIJUN_CENTER_X * 1414 / 1000) // 半径 * √2
                for (k = -LEN; k < LEN; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        return 0;
                    apt[0].x = LONG(KAKIJUN_CENTER_X + k * cost1 + LEN * sint1);
                    apt[0].y = LONG(KAKIJUN_CENTER_Y + k * sint1 - LEN * cost1);
                    apt[1].x = LONG(KAKIJUN_CENTER_X + k * cost1 - LEN * sint1);
                    apt[1].y = LONG(KAKIJUN_CENTER_Y + k * sint1 + LEN * cost1);
                    apt[2].x = LONG(KAKIJUN_CENTER_X + (k + 20) * cost1 - LEN * sint1);
                    apt[2].y = LONG(KAKIJUN_CENTER_Y + (k + 20) * sint1 + LEN * cost1);
                    apt[3].x = LONG(KAKIJUN_CENTER_X + (k + 20) * cost1 + LEN * sint1);
                    apt[3].y = LONG(KAKIJUN_CENTER_Y + (k + 20) * sint1 - LEN * cost1);
                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 4);
                    EndPath(hdcMem);
                    CRgn hRgn3(::PathToRegion(hdcMem));
                    CRgn hRgn4(::CreateRectRgn(0, 0, 0, 0));
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    if (n != NULLREGION)
                        break;
                }
                for ( ; k < LEN; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        break;
                    hbm1.Swap(hbm2);
                    g_hbmKakijun = hbm1;
                    hbmOld = SelectObject(hdcMem, hbm1);
                    apt[0].x = LONG(KAKIJUN_CENTER_X + k * cost1 + LEN * sint1);
                    apt[0].y = LONG(KAKIJUN_CENTER_Y + k * sint1 - LEN * cost1);
                    apt[1].x = LONG(KAKIJUN_CENTER_X + k * cost1 - LEN * sint1);
                    apt[1].y = LONG(KAKIJUN_CENTER_Y + k * sint1 + LEN * cost1);
                    apt[2].x = LONG(KAKIJUN_CENTER_X + (k + 20) * cost1 - LEN * sint1);
                    apt[2].y = LONG(KAKIJUN_CENTER_Y + (k + 20) * sint1 + LEN * cost1);
                    apt[3].x = LONG(KAKIJUN_CENTER_X + (k + 20) * cost1 + LEN * sint1);
                    apt[3].y = LONG(KAKIJUN_CENTER_Y + (k + 20) * sint1 - LEN * cost1);
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
                break;
            }

        case POLAR:
            {
                CDC hdc(g_hKakijunWnd);
                CDC hdcMem(hdc);
                hbm1.Swap(hbm2);
                g_hbmKakijun = hbm1;
                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = 0;
                rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                {
                    hFontOld = SelectObject(hdcMem, hFont);
                    SetTextColor(hdcMem, RGB(0, 0, 0));
                    SetBkColor(hdcMem, RGB(255, 255, 255));
                    SetBkMode(hdcMem, OPAQUE);
                    DrawTextA(hdcMem, romaji, lstrlenA(romaji), &rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
                    SelectObject(hdcMem, hFontOld);
                }
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v[i].res));

                INT step = 0;
                for (; step < 300 / 20; ++step)
                {
                    CRgn hRgn8(::CreateRectRgn(0, 0, 0, 0));
                    CRgn hRgn9(::CreateEllipticRgn(v[i].cx - 20 * step, v[i].cy - 20 * step, v[i].cx + 20 * step, v[i].cy + 20 * step));
                    if (CombineRgn(hRgn8, hRgn2, hRgn9, RGN_AND) != NULLREGION)
                        break;
                };
                INT dk = 100 / (step + 2);

                BOOL found = FALSE;
                INT sign = (v[i].angle0 <= v[i].angle1) ? +1 : -1;
                INT k0 = v[i].angle0, k1 = v[i].angle1;
                for (k = k0; k * sign <= k1 * sign; k += dk * sign)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        return 0;

                    double theta = k * M_PI / 180.0;
                    double theta2 = (k + dk * sign) * M_PI / 180.0;
                    double cost1 = cos(theta);
                    double sint1 = sin(theta);
                    double cost2 = cos(theta2);
                    double sint2 = sin(theta2);
                    double cost3 = cos((2 * theta + 1 * theta2) / 3);
                    double sint3 = sin((2 * theta + 1 * theta2) / 3);
                    double cost4 = cos((1 * theta + 2 * theta2) / 3);
                    double sint4 = sin((1 * theta + 2 * theta2) / 3);
                    hbm1.Swap(hbm2);

                    hbmOld = SelectObject(hdcMem, hbm1);

                    apt[0].x = LONG(v[i].cx + LEN * cost1);
                    apt[0].y = LONG(v[i].cy + LEN * sint1);
                    apt[1].x = LONG(v[i].cx + LEN * cost3);
                    apt[1].y = LONG(v[i].cy + LEN * sint3);
                    apt[2].x = LONG(v[i].cx + LEN * cost4);
                    apt[2].y = LONG(v[i].cy + LEN * sint4);
                    apt[3].x = LONG(v[i].cx + LEN * cost2);
                    apt[3].y = LONG(v[i].cy + LEN * sint2);
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

                    g_hbmKakijun = hbm1;
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
                break;
            }
        }
    }

    DO_SLEEP(500);
    MyPlaySoundAsync(MAKEINTRESOURCE(3000 + g_nMoji));

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);
        hbm1.Swap(hbm2);
        g_hbmKakijun = hbm1;
        hbmOld = SelectObject(hdcMem, hbm1);
        rc.left = 0;
        rc.top = 0;
        rc.right = siz.cx;
        rc.bottom = siz.cy;
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        {
            hFontOld = SelectObject(hdcMem, hFont);
            SetTextColor(hdcMem, RGB(0, 0, 0));
            SetBkColor(hdcMem, RGB(255, 255, 255));
            SetBkMode(hdcMem, OPAQUE);
            DrawTextA(hdcMem, romaji, lstrlenA(romaji), &rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
            SelectObject(hdcMem, hFontOld);
        }
        SelectObject(hdcMem, hbmOld);
    }

    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
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

    GetWindowRect(hwnd, &rc);
    GetWindowRect(g_hKakijunWnd, &rc2);
    MoveWindow(g_hKakijunWnd,
        rc.left + (rc.right - rc.left - (rc2.right - rc2.left)) / 2,
        rc.top + (rc.bottom - rc.top - (rc2.bottom - rc2.top)) / 2,
        rc2.right - rc2.left,
        rc2.bottom - rc2.top,
        TRUE);
    g_nMoji = nMoji;

    if (fRight)
    {
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, 3000, LoadStringDx(3000));
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, 100 + nMoji, LoadStringDx(1000 + g_nMoji));
        SetForegroundWindow(hwnd);

        POINT pt;
        GetCursorPos(&pt);

        INT nCmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
        if (nCmd)
            PostMessage(hwnd, WM_COMMAND, nCmd, 0);
        return;
    }

    if (g_fKatakana)
        g_katakana_history.insert(nMoji);
    else
        g_hiragana_history.insert(nMoji);

    if (g_hbmClient)
        DeleteObject(g_hbmClient);
    g_hbmClient = NULL;
    InvalidateRect(hwnd, NULL, FALSE);

    if (g_hThread != NULL)
        CloseHandle(g_hThread);
    g_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
}

// 「ひらがな」ボタンの当たり判定。
BOOL HitHiraganaRect(HWND hwnd, LPRECT prc, POINT pt)
{
    SetRect(prc, 160, 10, 160 + 200, 10 + 76);
    return PtInRect(prc, pt);
}

// 「カタカナ」ボタンの当たり判定。
BOOL HitKatakanaRect(HWND hwnd, LPRECT prc, POINT pt)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    SetRect(prc, rc.right - (160 + 200), 10, rc.right - 160, 10 + 76);
    return PtInRect(prc, pt);
}

VOID OnButtonDown(HWND hwnd, INT x, INT y, BOOL fRight)
{
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

    if (HitHiraganaRect(hwnd, &rc, pt))
    {
        g_fKatakana = FALSE;
        MyPlaySoundAsync(MAKEINTRESOURCE(300));
        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    if (HitKatakanaRect(hwnd, &rc, pt))
    {
        g_fKatakana = TRUE;
        MyPlaySoundAsync(MAKEINTRESOURCE(350));
        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    // 文字ボタンの当たり判定。
    for (UINT j = 0; j < _countof(g_ahbmMoji); ++j)
    {
        rc.left = g_moji_data[j].x;
        rc.top = g_moji_data[j].y;
        rc.right = rc.left + 50;
        rc.bottom = rc.top + 50;
        InflateRect(&rc, +3, +3);
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, g_moji_data[j].moji_id, fRight);
            return;
        }
        InflateRect(&rc, -3, -3);
    }
}

BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
    RECT rc;

    if (codeHitTest != HTCLIENT)
        return FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, msg, DefWindowProc);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);

    if (HitHiraganaRect(hwnd, &rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    if (HitKatakanaRect(hwnd, &rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    // 文字ボタンの当たり判定。
    for (UINT j = 0; j < _countof(g_ahbmMoji); ++j)
    {
        rc.left = g_moji_data[j].x;
        rc.top = g_moji_data[j].y;
        rc.right = rc.left + 50;
        rc.bottom = rc.top + 50;
        InflateRect(&rc, +3, +3);
        if (PtInRect(&rc, pt))
        {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
        InflateRect(&rc, -3, -3);
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return TRUE;
}

BOOL Kakijun_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
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

void Kakijun_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        Kakijun_OnDraw(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
}

LRESULT CALLBACK
KakijunWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_ERASEBKGND, Kakijun_OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, Kakijun_OnPaint);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        OnDraw(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    OnButtonDown(hwnd, x, y, FALSE);
}

void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    OnButtonDown(hwnd, x, y, TRUE);
}

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

void OnDestroy(HWND hwnd)
{
    if (g_hThread)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
        CloseHandle(g_hThread);
    }

    for (UINT i = 0; i < _countof(g_ahbmMoji); ++i)
    {
        if (g_ahbmMoji[i])
            DeleteObject(g_ahbmMoji[i]);
    }

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmKakijun);

    DeleteObject(g_hbrRed);
    DeleteObject(g_hbmHiragana);
    DeleteObject(g_hbmHiragana2);
    DeleteObject(g_hbmKatakana);
    DeleteObject(g_hbmKatakana2);

    g_katakana_history.clear();
    g_hiragana_history.clear();

    PostQuitMessage(0);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (id == 0)
        return;

    if (g_fKatakana)
        g_katakana_history.insert(g_nMoji);
    else
        g_hiragana_history.insert(g_nMoji);

    if (g_hbmClient)
    {
        DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
    }
    InvalidateRect(hwnd, NULL, TRUE);

    INT index = MojiIndexFromMojiID(g_nMoji);

    if (id == 3000)
    {
        WCHAR wch = g_moji_data[index + (g_fKatakana ? 46 : 0)].wch;
        WCHAR sz[2] = { wch, 0 };
        CopyText(hwnd, sz);
        return;
    }

    LPTSTR psz = LoadStringDx(2000 + g_nMoji);
    if (psz[0])
        ShellExecute(hwnd, NULL, psz, NULL, NULL, SW_SHOWNORMAL);
}

void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (!fDown)
        return;

    if (vk == VK_ESCAPE)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
    }
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
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SYSCOMMAND, OnSysCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKey);
        HANDLE_MSG(hwnd, WM_SETCURSOR, OnSetCursor);
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
        return 0;
    }

    // ウィンドウクラスを登録する。
    WNDCLASSEX wcx = { sizeof(wcx) };
    wcx.style           = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
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
    RECT rc = { 0, 0, 774, 401 };
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
    while((f = GetMessage(&msg, NULL, 0, 0)) != FALSE)
    {
        if (f == -1)
            return -1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Detect handle leaks
    OBJECTS_CHECK_POINT();

    // Detect memory leaks
#if defined(_MSC_VER) && !defined(NDEBUG) // Visual C++ only
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return (INT)msg.wParam;
}
