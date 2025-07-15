// Moji No Benkyo (3)
// Copyright (C) 2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <process.h>
#include <tchar.h>
#include <cstring>

#include <cstdlib>
#include <process.h>
#include <cmath>

#include <new>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "kakijun.h"

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (3)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (3) Kakijun");
static const TCHAR g_szCaptionClassName[] = TEXT("Moji No Benkyou (3) Caption");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;

HBITMAP g_ahbmKanji1[80];
HBITMAP g_hbm;

HBITMAP g_hbm2;
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;

std::set<INT> g_kanji1_history;

LPTSTR LoadStringDx(INT ids)
{
    static TCHAR sz[512];
    ZeroMemory(sz, sizeof(sz));
    LoadString(g_hInstance, ids, sz, 512);
    return sz;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    INT j;
    HMENU hSysMenu;
    MENUITEMINFO mii;

    g_hThread = NULL;
    g_hbm2 = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

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

    ZeroMemory(g_ahbmKanji1, sizeof(g_ahbmKanji1));
    for (j = 0; j < 80; ++j)
    {
        int ix = j % 10;
        int iy = j / 10;
        g_ahbmKanji1[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100 + j));
        if (g_ahbmKanji1[j] == NULL)
            return FALSE;
    }

    g_hbm = NULL;

    try
    {
        InitKanji1();
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
    HDC hdcMem, hdcMem2;
    HGDIOBJ hbmOld, hbmOld2;
    INT j;
    RECT rc;
    SIZE siz;
    HBRUSH hbr;

    hdcMem = CreateCompatibleDC(hdc);
    hdcMem2 = CreateCompatibleDC(hdc);

    GetClientRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;
    if (g_hbm == NULL)
    {
        g_hbm = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbmOld2 = SelectObject(hdcMem2, g_hbm);
        hbr = CreateSolidBrush(RGB(255, 255, 192));
        FillRect(hdcMem2, &rc, hbr);
        DeleteObject(hbr);

        for (j = 0; j < 80; ++j)
        {
            int ix = j % 10;
            int iy = j / 10;
            hbmOld = SelectObject(hdcMem, g_ahbmKanji1[g_map[j]]);
            rc.left = ix * (50 + 10) + 5 + 25;
            rc.top = iy * (50 + 10) + 5 + 10;
            if (iy >= 4)
                rc.top += 15;
            rc.right = rc.left + (50 + 10) - 10;
            rc.bottom = rc.top + (50 + 10) - 10;
            if (g_kanji1_history.find(j) != g_kanji1_history.end())
                FillRect(hdcMem2, &rc, g_hbrRed);
            else
                FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            InflateRect(&rc, -5, -5);
            BitBlt(hdcMem2, rc.left, rc.top, 40, 40, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        SelectObject(hdcMem2, hbmOld2);
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbm);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);
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

unsigned __stdcall ThreadProc( void * )
{
    RECT rc;
    SIZE siz;
    HDC hdc, hdcMem;
    HBITMAP hbm, hbm2, hbmTemp;
    HGDIOBJ hbmOld;
    HRGN hRgn, hRgn2, hRgn3, hRgn4, hRgn5;
    std::vector<GA> v;
    INT k;
    POINT apt[4];
    double cost, sint, cost2, sint2;

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    v = g_kanji1_kakijun[g_nMoji];

    hRgn = CreateRectRgn(0, 0, 0, 0);
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].pb != NULL)
        {
            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
            DeleteObject(hRgn2);
        }
    }

    hdc = GetDC(g_hKakijunWnd);
    hdcMem = CreateCompatibleDC(hdc);
    hbm = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
    hbm2 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);

    hbmOld = SelectObject(hdcMem, hbm);
    rc.left = 0;
    rc.top = 0;
    rc.right = siz.cx;
    rc.bottom = siz.cy;
    FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(g_hKakijunWnd, hdc);

    g_hbm2 = hbm;
    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);
    Sleep(300);

    hRgn5 = CreateRectRgn(0, 0, 0, 0);
    PlaySound(MAKEINTRESOURCE(1000 + 10 * g_nMoji), g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
    PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    for (UINT i = 0; i < v.size(); i++)
    {
        switch (v[i].type)
        {
        case WAIT:
            Sleep(500);
            PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
            break;

        case LINEAR:
            hdc = GetDC(g_hKakijunWnd);
            hdcMem = CreateCompatibleDC(hdc);
            hbmTemp = hbm;
            hbm = hbm2;
            hbm2 = hbmTemp;
            g_hbm2 = hbm;
            hbmOld = SelectObject(hdcMem, hbm);
            rc.left = 0;
            rc.top = 0;
            rc.right = siz.cx;
            rc.bottom = siz.cy;
            FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
            FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SelectObject(hdcMem, hbmOld);

            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            cost = cos(v[i].angle0 * M_PI / 180);
            sint = sin(v[i].angle0 * M_PI / 180);
            for (k = -200; k < 200; k += 20)
            {
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
                hRgn3 = PathToRegion(hdcMem);
                hRgn4 = CreateRectRgn(0, 0, 0, 0);
                INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                if (n != NULLREGION)
                {
                    DeleteObject(hRgn4);
                    break;
                }
                DeleteObject(hRgn4);
            }
            for ( ; k < 200; k += 20)
            {
                hbmTemp = hbm;
                hbm = hbm2;
                hbm2 = hbmTemp;
                g_hbm2 = hbm;
                hbmOld = SelectObject(hdcMem, hbm);
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
                hRgn3 = PathToRegion(hdcMem);
                hRgn4 = CreateRectRgn(0, 0, 0, 0);
                INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                CombineRgn(hRgn5, hRgn5, hRgn4, RGN_OR);
                FillRgn(hdcMem, hRgn5, g_hbrRed);
                DeleteObject(hRgn4);
                SelectObject(hdcMem, hbmOld);

                InvalidateRect(g_hKakijunWnd, NULL, FALSE);
                if (n == NULLREGION)
                    break;
                Sleep(35);
            }
            DeleteObject(hRgn2);
            DeleteDC(hdcMem);
            ReleaseDC(g_hKakijunWnd, hdc);
            break;

        case POLAR:
            hdc = GetDC(g_hKakijunWnd);
            hdcMem = CreateCompatibleDC(hdc);
            hbmTemp = hbm;
            hbm = hbm2;
            hbm2 = hbmTemp;
            g_hbm2 = hbm;
            hbmOld = SelectObject(hdcMem, hbm);
            rc.left = 0;
            rc.top = 0;
            rc.right = siz.cx;
            rc.bottom = siz.cy;
            FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
            FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SelectObject(hdcMem, hbmOld);

            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            if (v[i].angle0 <= v[i].angle1)
            {
                for (k = v[i].angle0; k < v[i].angle1; k += 20)
                {
                    double theta = k * M_PI / 180.0;
                    double theta2 = (k + 20) * M_PI / 180.0;
                    cost = cos(theta);
                    sint = sin(theta);
                    cost2 = cos(theta2);
                    sint2 = sin(theta2);
                    hbmTemp = hbm;
                    hbm = hbm2;
                    hbm2 = hbmTemp;
                    g_hbm2 = hbm;
                    hbmOld = SelectObject(hdcMem, hbm);
                    apt[0].x = LONG(v[i].cx + 200 * cost);
                    apt[0].y = LONG(v[i].cy + 200 * sint);
                    apt[1].x = LONG(v[i].cx + 200 * cost2);
                    apt[1].y = LONG(v[i].cy + 200 * sint2);
                    apt[2].x = v[i].cx;
                    apt[2].y = v[i].cy;
                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 3);
                    EndPath(hdcMem);
                    hRgn3 = PathToRegion(hdcMem);
                    hRgn4 = CreateRectRgn(0, 0, 0, 0);
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    CombineRgn(hRgn5, hRgn5, hRgn4, RGN_OR);
                    FillRgn(hdcMem, hRgn5, g_hbrRed);
                    DeleteObject(hRgn4);
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
                    double theta = (k - 20) * M_PI / 180.0;
                    double theta2 = k * M_PI / 180.0;
                    cost = cos(theta);
                    sint = sin(theta);
                    cost2 = cos(theta2);
                    sint2 = sin(theta2);
                    hbmTemp = hbm;
                    hbm = hbm2;
                    hbm2 = hbmTemp;
                    g_hbm2 = hbm;
                    hbmOld = SelectObject(hdcMem, hbm);
                    apt[0].x = LONG(v[i].cx + 200 * cost);
                    apt[0].y = LONG(v[i].cy + 200 * sint);
                    apt[1].x = LONG(v[i].cx + 200 * cost2);
                    apt[1].y = LONG(v[i].cy + 200 * sint2);
                    apt[2].x = v[i].cx;
                    apt[2].y = v[i].cy;
                    BeginPath(hdcMem);
                    Polygon(hdcMem, apt, 3);
                    EndPath(hdcMem);
                    hRgn3 = PathToRegion(hdcMem);
                    hRgn4 = CreateRectRgn(0, 0, 0, 0);
                    INT n = CombineRgn(hRgn4, hRgn2, hRgn3, RGN_AND);
                    CombineRgn(hRgn5, hRgn5, hRgn4, RGN_OR);
                    FillRgn(hdcMem, hRgn5, g_hbrRed);
                    DeleteObject(hRgn4);
                    SelectObject(hdcMem, hbmOld);

                    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
                    if (n == NULLREGION)
                        break;
                    Sleep(35);
                }
            }
            DeleteObject(hRgn2);
            DeleteDC(hdcMem);
            ReleaseDC(g_hKakijunWnd, hdc);
            break;
        }
    }
    DeleteObject(hRgn5);

    Sleep(500);

    hdc = GetDC(g_hKakijunWnd);
    hdcMem = CreateCompatibleDC(hdc);
    hbmTemp = hbm;
    hbm = hbm2;
    hbm2 = hbmTemp;
    g_hbm2 = hbm;
    hbmOld = SelectObject(hdcMem, hbm);
    rc.left = 0;
    rc.top = 0;
    rc.right = siz.cx;
    rc.bottom = siz.cy;
    FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(g_hKakijunWnd, hdc);

    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    PlaySound(MAKEINTRESOURCE(1000 + 10 * g_nMoji), g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
    Sleep(500);

    ShowWindow(g_hKakijunWnd, SW_HIDE);
    g_hbm2 = NULL;
    DeleteObject(hbm);
    DeleteObject(hbm2);
    return 0;
}

VOID MojiOnClick(HWND hwnd, INT nMoji, BOOL fRight)
{
    RECT rc, rc2;
    g_nMoji = g_map[nMoji];

    if (fRight)
    {
        SetForegroundWindow(hwnd);
        HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(100));
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        POINT pt;
        GetCursorPos(&pt);
        INT nCmd = TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  pt.x, pt.y, 0, hwnd, NULL);
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

    LPTSTR psz = LoadStringDx(100 + g_nMoji);
    LPTSTR pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption1, (pch + 1));

    psz = LoadStringDx(200 + g_nMoji);
    pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption2, (pch + 1));

    g_kanji1_history.insert(nMoji);

    if (g_hbm != NULL)
        DeleteObject(g_hbm);

    g_hbm = NULL;
    InvalidateRect(hwnd, NULL, FALSE);

    PlaySound(MAKEINTRESOURCE(3000 + nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    if (g_hThread != NULL)
    {
        TerminateThread(g_hThread, 0);
        CloseHandle(g_hThread);
    }
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
    for (j = 0; j < 80; ++j)
    {
        int ix = j % 10;
        int iy = j / 10;
        rc.left = ix * (50 + 10) + 5 + 25;
        rc.top = iy * (50 + 10) + 5 + 10;
        if (iy >= 4)
            rc.top += 15;
        rc.right = rc.left + (50 + 10) - 10;
        rc.bottom = rc.top + (50 + 10) - 10;
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, j, fRight);
            return;
        }
    }
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
    if (g_hbm2 != NULL)
    {
        hdcMem = CreateCompatibleDC(hdc);
        hbmOld = SelectObject(hdcMem, g_hbm2);
        BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
    }
}

void Caption_OnPaint(HWND hwnd)
{
    TCHAR szText[256];
    GetWindowText(hwnd, szText, 256);
    RECT rc;
    GetClientRect(hwnd, &rc);
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        if (lstrlen(szText) > 25)
        {
            HGDIOBJ hFontOld = SelectObject(hdc, g_hFontSmall);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawText(hdc, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, hFontOld);
        }
        else
        {
            HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawText(hdc, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
            SelectObject(hdc, hFontOld);
        }
        EndPaint(hwnd, &ps);
    }
}

LRESULT CALLBACK
CaptionWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_PAINT, Caption_OnPaint);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL Kakijun_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hwndCaption1 = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        g_szCaptionClassName, NULL,
        WS_POPUP | WS_BORDER, 0, 0, 0, 0,
        GetParent(hwnd), NULL, g_hInstance, NULL);

    g_hwndCaption2 = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        g_szCaptionClassName, NULL,
        WS_POPUP | WS_BORDER, 0, 0, 0, 0,
        GetParent(hwnd), NULL, g_hInstance, NULL);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lstrcpyn(lf.lfFaceName, TEXT("Piza P Gothic"), _countof(lf.lfFaceName));
    lf.lfHeight = -35;
    lf.lfWeight = FW_BOLD;
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lf.lfQuality = ANTIALIASED_QUALITY;
    g_hFont = CreateFontIndirect(&lf);

    lf.lfHeight = -15;
    g_hFontSmall = CreateFontIndirect(&lf);

    return g_hFont != NULL;
}

void MoveCaptionWnd(HWND hwnd, HWND hwndCaption, INT nIndex)
{
    RECT rc;
    GetWindowRect(hwnd, &rc);

    TCHAR szText[256];
    GetWindowText(hwndCaption, szText, 256);

    SIZE siz;

    if (HDC hdc = GetDC(hwndCaption))
    {
        if (lstrlen(szText) > 25)
        {
            HGDIOBJ hFontOld = SelectObject(hdc, g_hFontSmall);
            GetTextExtentPoint32(hdc, szText, lstrlen(szText), &siz);
            SelectObject(hdc, hFontOld);
        }
        else
        {
            HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
            GetTextExtentPoint32(hdc, szText, lstrlen(szText), &siz);
            SelectObject(hdc, hFontOld);
        }
        ReleaseDC(hwndCaption, hdc);
    }
    siz.cx += 16;

    if (nIndex == 0)
    {
        MoveWindow(hwndCaption, (rc.left + rc.right - siz.cx) / 2, rc.top - 50, siz.cx, 40, TRUE);
    }
    else
    {
        MoveWindow(hwndCaption, (rc.left + rc.right - siz.cx) / 2, rc.bottom + 10, siz.cx, 40, TRUE);
    }
}

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

BOOL Kakijun_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
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

void Kakijun_OnDestroy(HWND hwnd)
{
    DestroyWindow(g_hwndCaption1);
    g_hwndCaption1 = NULL;
    DestroyWindow(g_hwndCaption2);
    g_hwndCaption2 = NULL;
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

void OnDestroy(HWND hwnd)
{
        if (g_hThread != NULL)
        {
            TerminateThread(g_hThread, 0);
            CloseHandle(g_hThread);
        }
        PostQuitMessage(0);
}

BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (fDoubleClick)
        return;

    OnButtonDown(hwnd, x, y, FALSE);
}

void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (fDoubleClick)
        return;

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

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    WCHAR wch;
    TCHAR szText[MAX_PATH];
    tstring str;

    switch (id)
    {
    case 1000:
        {
            LPTSTR psz = LoadStringDx(100 + g_nMoji);
            LPTSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
            str = psz;
            wsprintf(szText, LoadStringDx(1000), str.c_str());
            ShellExecute(hwnd, NULL, szText, NULL, NULL, SW_SHOWNORMAL);
        }
        break;

    case 1001:
        {
            LPTSTR psz = LoadStringDx(100 + g_nMoji);
            LPTSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
#ifdef UNICODE
            wch = *psz;
#else
            MultiByteToWideChar(CP_ACP, 0, psz, -1, sz, 2);
            wch = sz[0];
#endif
            wsprintf(szText, LoadStringDx(1001), wch);
            ShellExecute(hwnd, NULL, szText, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
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
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDBLCLK, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SYSCOMMAND, OnSysCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);

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
    DWORD style;
    MSG msg;
    BOOL f;

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

    wcx.style           = CS_NOCLOSE | CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc     = CaptionWndProc;
    wcx.hIcon           = NULL;
    wcx.hbrBackground   = CreateSolidBrush(RGB(255, 255, 64));
    wcx.lpszClassName   = g_szCaptionClassName;
    wcx.hIconSm         = NULL;
    if (!RegisterClassEx(&wcx))
        return 1;

    style = WS_SYSMENU | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX;
    g_hMainWnd = CreateWindow(g_szClassName, LoadStringDx(1), style,
        CW_USEDEFAULT, CW_USEDEFAULT, 660, 550,
        NULL, NULL, hInstance, NULL);
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
