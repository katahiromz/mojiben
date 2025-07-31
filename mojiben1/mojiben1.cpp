// Moji No Benkyo (1)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <mmsystem.h>

#include <cstdlib>
#include <process.h>
#include <cmath>

#include <new>
#include <vector>
#include <map>
#include <set>

#include "kakijun.h"
#include "../CGdiObj.h"

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (1)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (1) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

HBITMAP g_hbmHiragana, g_hbmHiragana2;
HBITMAP g_hbmKatakana, g_hbmKatakana2;
HBITMAP g_aahbmHiragana[11][5];
HBITMAP g_aahbmKatakana[11][5];
HBITMAP g_hbmClient;
BOOL g_fKatakana;

HBITMAP g_hbmKakijun = NULL;
INT g_nMoji;
HANDLE g_hThread = NULL;
HBRUSH g_hbrRed = NULL;

std::set<INT> g_katakana_history;
std::set<INT> g_hiragana_history;

LPTSTR LoadStringDx(INT ids)
{
    static TCHAR sz[512];
    LoadString(g_hInstance, ids, sz, 512);
    return sz;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    INT i, j;
    HMENU hSysMenu;
    MENUITEMINFO mii;

    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));
    g_hbmHiragana = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100));
    g_hbmHiragana2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(150));
    g_hbmKatakana = LoadBitmap(g_hInstance, MAKEINTRESOURCE(200));
    g_hbmKatakana2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(250));
    g_fKatakana = FALSE;

    hSysMenu = GetSystemMenu(hwnd, FALSE);
    ZeroMemory(&mii, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE;
    mii.fType = MFT_SEPARATOR;
    mii.fState = MFS_ENABLED;
    mii.wID = (UINT)-1;
    mii.dwTypeData = 0;
    InsertMenuItem(hSysMenu, -1, TRUE, &mii);
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE;
    mii.fType = MFT_STRING;
    mii.fState = MFS_ENABLED;
    mii.wID = 0x3330;
    mii.dwTypeData = LoadStringDx(2);
    InsertMenuItem(hSysMenu, -1, TRUE, &mii);

    ZeroMemory(g_aahbmHiragana, sizeof(g_aahbmHiragana));
    for (j = 0; j <= 100; j += 10)
    {
        if (j == 70)
        {
            i = 0;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
            i = 2;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
            i = 4;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
        }
        else if (j == 90)
        {
            i = 0;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
            i = 4;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
        }
        else if (j == 100)
        {
            i = 4;
            g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
            if (g_aahbmHiragana[j / 10][i] == NULL)
                return FALSE;
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                g_aahbmHiragana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j + i));
                if (g_aahbmHiragana[j / 10][i] == NULL)
                    return FALSE;
            }
        }
    }

    ZeroMemory(g_aahbmKatakana, sizeof(g_aahbmKatakana));
    for (j = 0; j <= 100; j += 10)
    {
        if (j == 70)
        {
            i = 0;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
            i = 2;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
            i = 4;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
        }
        else if (j == 90)
        {
            i = 0;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
            i = 4;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
        }
        else if (j == 100)
        {
            i = 4;
            g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
            if (g_aahbmKatakana[j / 10][i] == NULL)
                return FALSE;
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                g_aahbmKatakana[j / 10][i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + j + i));
                if (g_aahbmKatakana[j / 10][i] == NULL)
                    return FALSE;
            }
        }
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
    INT i, j;
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

        typedef HBITMAP MYBITMAPS[11][5];
        MYBITMAPS *bitmaps;
        if (g_fKatakana)
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiragana2);
            BitBlt(hdcMem2, 160, 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakana);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            bitmaps = &g_aahbmKatakana;
        }
        else
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiragana);
            BitBlt(hdcMem2, 160, 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakana2);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 10, 200, 76, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            bitmaps = &g_aahbmHiragana;
        }

        for (j = 0; j <= 100; j += 10)
        {
            if (j == 70)
            {
                i = 0;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
                i = 2;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
                i = 4;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
            }
            else if (j == 90)
            {
                i = 0;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
                i = 4;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
            }
            else if (j == 100)
            {
                i = 4;
                hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                    rc.left = 685 - (j * 65) / 10 - 3;
                    rc.top = i * 60 + 100 - 3;
                    rc.right = rc.left + 50 + 6;
                    rc.bottom = rc.top + 50 + 6;
                if (g_fKatakana)
                {
                    if (g_katakana_history.find(j + i) != g_katakana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                else
                {
                    if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                        FillRect(hdcMem2, &rc, g_hbrRed);
                    else
                        FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                }
                BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
            }
            else
            {
                for (i = 0; i < 5; i++)
                {
                    hbmOld = SelectObject(hdcMem, (*bitmaps)[j / 10][i]);
                        rc.left = 685 - (j * 65) / 10 - 3;
                        rc.top = i * 60 + 100 - 3;
                        rc.right = rc.left + 50 + 6;
                        rc.bottom = rc.top + 50 + 6;
                    if (g_fKatakana)
                    {
                        if (g_katakana_history.find(j + i) != g_katakana_history.end())
                            FillRect(hdcMem2, &rc, g_hbrRed);
                        else
                            FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                    }
                    else
                    {
                        if (g_hiragana_history.find(j + i) != g_hiragana_history.end())
                            FillRect(hdcMem2, &rc, g_hbrRed);
                        else
                            FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
                    }
                    BitBlt(hdcMem2, 685 - (j * 65) / 10, i * 60 + 100, 70, 70, hdcMem, 0, 0, SRCCOPY);
                    SelectObject(hdcMem, hbmOld);
                }
            }
        }
        SelectObject(hdcMem2, hbmOld2);
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
}

unsigned __stdcall ThreadProc( void * )
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    std::vector<GA> v;
    INT k;
    POINT apt[4];
    double cost, sint, cost2, sint2;
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

    if (g_fKatakana)
        v = g_katakana_kakijun[g_nMoji];
    else
        v = g_hiragana_kakijun[g_nMoji];

    switch (g_nMoji / 10)
    {
    case 0:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "a"; break;
        case 1: romaji = "i"; break;
        case 2: romaji = "u"; break;
        case 3: romaji = "e"; break;
        case 4: romaji = "o"; break;
        }
        break;
    case 1:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ka"; break;
        case 1: romaji = "ki"; break;
        case 2: romaji = "ku"; break;
        case 3: romaji = "ke"; break;
        case 4: romaji = "ko"; break;
        }
        break;
    case 2:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "sa"; break;
        case 1: romaji = "shi"; break;
        case 2: romaji = "su"; break;
        case 3: romaji = "se"; break;
        case 4: romaji = "so"; break;
        }
        break;
    case 3:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ta"; break;
        case 1: romaji = "chi"; break;
        case 2: romaji = "tsu (tz)"; break;
        case 3: romaji = "te"; break;
        case 4: romaji = "to"; break;
        }
        break;
    case 4:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "na"; break;
        case 1: romaji = "ni"; break;
        case 2: romaji = "nu"; break;
        case 3: romaji = "ne"; break;
        case 4: romaji = "no"; break;
        }
        break;
    case 5:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ha (wa)"; break;
        case 1: romaji = "hi"; break;
        case 2: romaji = "fu"; break;
        case 3: romaji = "he"; break;
        case 4: romaji = "ho"; break;
        }
        break;
    case 6:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ma"; break;
        case 1: romaji = "mi"; break;
        case 2: romaji = "mu"; break;
        case 3: romaji = "me"; break;
        case 4: romaji = "mo"; break;
        }
        break;
    case 7:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ya"; break;
        case 2: romaji = "yu"; break;
        case 4: romaji = "yo"; break;
        }
        break;
    case 8:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "ra (la)"; break;
        case 1: romaji = "ri (li)"; break;
        case 2: romaji = "ru (lu)"; break;
        case 3: romaji = "re (le)"; break;
        case 4: romaji = "ro (lo)"; break;
        }
        break;
    case 9:
        switch (g_nMoji % 10)
        {
        case 0: romaji = "wa"; break;
        case 4: romaji = "wo (o)"; break;
        }
        break;
    case 10:
        switch (g_nMoji % 10)
        {
        case 4: romaji = "nn (n/m)"; break;
        }
        break;
    }

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].type != WAIT && v[i].pb)
        {
            CRgn hRgn2(::ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb));
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
    Sleep(300);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    PlaySound(MAKEINTRESOURCE(400), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    for (UINT i = 0; i < v.size(); i++)
    {
        switch (v[i].type)
        {
        case WAIT:
            Sleep(500);
            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;
            PlaySound(MAKEINTRESOURCE(400), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
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

                CRgn hRgn2(::ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb));
                cost = cos(v[i].angle0 * M_PI / 180);
                sint = sin(v[i].angle0 * M_PI / 180);
                for (k = -200; k < 200; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        return 0;
                    apt[0].x = LONG(150 + k * cost + 150 * sint);
                    apt[0].y = LONG(150 + k * sint - 150 * cost);
                    apt[1].x = LONG(150 + k * cost - 150 * sint);
                    apt[1].y = LONG(150 + k * sint + 150 * cost);
                    apt[2].x = LONG(150 + (k + 20) * cost - 150 * sint);
                    apt[2].y = LONG(150 + (k + 20) * sint + 150 * cost);
                    apt[3].x = LONG(150 + (k + 20) * cost + 150 * sint);
                    apt[3].y = LONG(150 + (k + 20) * sint - 150 * cost);
                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 4);
                    EndPath(hdcMem);
                    CRgn hRgn3(::PathToRegion(hdcMem));
                    CRgn hRgn4(::CreateRectRgn(0, 0, 0, 0));
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    if (n != NULLREGION)
                        break;
                }
                for ( ; k < 200; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        break;
                    hbm1.Swap(hbm2);
                    g_hbmKakijun = hbm1;
                    hbmOld = SelectObject(hdcMem, hbm1);
                    apt[0].x = LONG(150 + k * cost + 150 * sint);
                    apt[0].y = LONG(150 + k * sint - 150 * cost);
                    apt[1].x = LONG(150 + k * cost - 150 * sint);
                    apt[1].y = LONG(150 + k * sint + 150 * cost);
                    apt[2].x = LONG(150 + (k + 20) * cost - 150 * sint);
                    apt[2].y = LONG(150 + (k + 20) * sint + 150 * cost);
                    apt[3].x = LONG(150 + (k + 20) * cost + 150 * sint);
                    apt[3].y = LONG(150 + (k + 20) * sint - 150 * cost);
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
                    Sleep(35);
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

                CRgn hRgn2(::ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb));
                if (v[i].angle0 <= v[i].angle1)
                {
                    for (k = v[i].angle0; k < v[i].angle1; k += 20)
                    {
                        if (!IsWindowVisible(g_hKakijunWnd))
                            return 0;
                        double theta = k * M_PI / 180.0;
                        double theta2 = (k + 20) * M_PI / 180.0;
                        cost = cos(theta);
                        sint = sin(theta);
                        cost2 = cos(theta2);
                        sint2 = sin(theta2);
                        hbm1.Swap(hbm2);
                        g_hbmKakijun = hbm1;
                        hbmOld = SelectObject(hdcMem, hbm1);
                        apt[0].x = LONG(v[i].cx + 200 * cost);
                        apt[0].y = LONG(v[i].cy + 200 * sint);
                        apt[1].x = LONG(v[i].cx + 200 * cost2);
                        apt[1].y = LONG(v[i].cy + 200 * sint2);
                        apt[2].x = v[i].cx;
                        apt[2].y = v[i].cy;
                        BeginPath(hdcMem);
                        Polygon(hdcMem, apt, 3);
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
                        Sleep(35);
                    }
                }
                else
                {
                    for (k = v[i].angle0; k > v[i].angle1; k -= 20)
                    {
                        if (!IsWindowVisible(g_hKakijunWnd))
                            return 0;
                        double theta = (k - 20) * M_PI / 180.0;
                        double theta2 = k * M_PI / 180.0;
                        cost = cos(theta);
                        sint = sin(theta);
                        cost2 = cos(theta2);
                        sint2 = sin(theta2);
                        hbm1.Swap(hbm2);
                        g_hbmKakijun = hbm1;
                        hbmOld = SelectObject(hdcMem, hbm1);
                        apt[0].x = LONG(v[i].cx + 200 * cost);
                        apt[0].y = LONG(v[i].cy + 200 * sint);
                        apt[1].x = LONG(v[i].cx + 200 * cost2);
                        apt[1].y = LONG(v[i].cy + 200 * sint2);
                        apt[2].x = v[i].cx;
                        apt[2].y = v[i].cy;
                        BeginPath(hdcMem);
                        Polygon(hdcMem, apt, 3);
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
                        Sleep(35);
                    }
                }
                break;
            }
        }
    }

    Sleep(500);
    PlaySound(MAKEINTRESOURCE(3000 + g_nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

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
    Sleep(500);

    ShowWindow(g_hKakijunWnd, SW_HIDE);
    g_hbmKakijun = NULL;
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
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, 100 + nMoji, LoadStringDx(1000 + g_nMoji));
        SetForegroundWindow(hwnd);

        POINT pt;
        GetCursorPos(&pt);

        INT nCmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  pt.x, pt.y, 0, hwnd, NULL);
        SendMessage(hwnd, WM_COMMAND, nCmd, 0);
        DestroyMenu(hMenu);
        return;
    }

    if (g_fKatakana)
        g_katakana_history.insert(nMoji);
    else
        g_hiragana_history.insert(nMoji);
    if (g_hbmClient != NULL)
        DeleteObject(g_hbmClient);
    g_hbmClient = NULL;
    InvalidateRect(hwnd, NULL, FALSE);

    PlaySound(MAKEINTRESOURCE(3000 + nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    if (g_hThread != NULL)
        CloseHandle(g_hThread);
    g_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
}

VOID OnButtonDown(HWND hwnd, INT x, INT y, BOOL fRight)
{
    INT i, j;
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
    SetRect(&rc, 160, 10, 160 + 200, 10 + 76);
    if (PtInRect(&rc, pt))
    {
        g_fKatakana = FALSE;
        PlaySound(MAKEINTRESOURCE(300), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        if (g_hbmClient != NULL)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }
    SetRect(&rc, siz.cx - (160 + 200), 10, siz.cx - 160, 10 + 76);
    if (PtInRect(&rc, pt))
    {
        g_fKatakana = TRUE;
        PlaySound(MAKEINTRESOURCE(350), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    for (j = 0; j <= 100; j += 10)
    {
        if (j == 70)
        {
            i = 0;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
            i = 2;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
            i = 4;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
        }
        else if (j == 90)
        {
            i = 0;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
            i = 4;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
        }
        else if (j == 100)
        {
            i = 4;
            rc.left = 685 - (j * 65) / 10 - 3;
            rc.top = i * 60 + 100 - 3;
            rc.right = rc.left + 50 + 6;
            rc.bottom = rc.top + 50 + 6;
            if (PtInRect(&rc, pt))
            {
                MojiOnClick(hwnd, j + i, fRight);
                return;
            }
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                rc.left = 685 - (j * 65) / 10 - 3;
                rc.top = i * 60 + 100 - 3;
                rc.right = rc.left + 50 + 6;
                rc.bottom = rc.top + 50 + 6;
                if (PtInRect(&rc, pt))
                {
                    MojiOnClick(hwnd, j + i, fRight);
                    return;
                }
            }
        }
    }
}

BOOL Kakijun_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

VOID Kakijun_OnDraw(HWND hwnd, HDC hdc)
{
    RECT rc;
    SIZE siz;
    HDC hdcMem;
    HGDIOBJ hbmOld;

    GetClientRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;
    if (g_hbmKakijun)
    {
        hdcMem = CreateCompatibleDC(hdc);
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

INT_PTR CALLBACK
AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, IDOK);
            break;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
    }
    return FALSE;
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
    if ((cmd & 0xFFF0) == 0x3330)
    {
        DialogBox(g_hInstance, MAKEINTRESOURCE(1), hwnd, AboutDialogProc);
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

    UINT i, j;
    for (i = 0; i < _countof(g_aahbmHiragana); ++i)
    {
        for (j = 0; j < _countof(g_aahbmHiragana[0]); ++j)
        {
            if (g_aahbmHiragana[i][j])
                DeleteObject(g_aahbmHiragana[i][j]);
        }
    }
    for (i = 0; i < _countof(g_aahbmKatakana); ++i)
    {
        for (j = 0; j < _countof(g_aahbmKatakana[0]); ++j)
        {
            if (g_aahbmKatakana[i][j])
                DeleteObject(g_aahbmKatakana[i][j]);
        }
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
    WNDCLASSEX wcx;
    MSG msg;
    BOOL f;

    InitCommonControls();

    RECT rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
    INT cxWork = (rcWorkArea.right - rcWorkArea.left);
    INT cyWork = (rcWorkArea.bottom - rcWorkArea.top);
    if (cxWork <= 780 || cyWork <= 430)
    {
        MessageBox(NULL, LoadStringDx(4), NULL, MB_ICONERROR);
        return 0;
    }

    g_hInstance = hInstance;
    wcx.cbSize          = sizeof(WNDCLASSEX);
    wcx.style           = 0;
    wcx.lpfnWndProc     = WindowProc;
    wcx.cbClsExtra      = 0;
    wcx.cbWndExtra      = 0;
    wcx.hInstance       = hInstance;
    wcx.hIcon           = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wcx.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground   = (HBRUSH)CreateSolidBrush(RGB(255, 255, 192));
    wcx.lpszMenuName    = NULL;
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

    DWORD style = WS_SYSMENU | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX;
    g_hMainWnd = CreateWindow(g_szClassName, LoadStringDx(1), style,
        CW_USEDEFAULT, CW_USEDEFAULT, 780, 430, NULL, NULL, hInstance, NULL);
    if (g_hMainWnd == NULL)
    {
        MessageBox(NULL, LoadStringDx(3), NULL, MB_ICONERROR);
        return 2;
    }

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    while((f = GetMessage(&msg, NULL, 0, 0)) != FALSE)
    {
        if (f == -1)
            return -1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (INT)msg.wParam;
}
