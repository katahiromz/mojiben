// Moji No Benkyou (4)
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

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (4)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (4) Kakijun");
static const TCHAR g_szCaptionClassName[] = TEXT("Moji No Benkyou (4) Caption");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;

HBITMAP g_ahbmKanji1[80];
HBITMAP g_hbmClient;

HBITMAP g_hbmKakijun;
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;

std::set<INT> g_kanji1_history;

BOOL g_bHighSpeed = FALSE;

extern "C" extern const LPCWSTR g_aszMojiReadings[];

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    updateSystemMenu(hwnd);

    ZeroMemory(g_ahbmKanji1, sizeof(g_ahbmKanji1));
    for (INT j = 0; j < 80; ++j)
    {
        int ix = j % 10;
        int iy = j / 10;
        g_ahbmKanji1[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100 + j));
        if (g_ahbmKanji1[j] == NULL)
            return FALSE;
    }

    g_hbmClient = NULL;

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

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
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
    std::vector<GA> v;
    INT k;
    POINT apt[4];
    double cost, sint, cost2, sint2;

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    v = g_kanji1_kakijun[g_nMoji];

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

        SelectObject(hdcMem, hbmOld);
    }

    g_hbmKakijun = hbm1;
    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);
    DoSleep(300);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    PlaySound(MAKEINTRESOURCE(1000 + g_nMoji), g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
    PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    for (UINT i = 0; i < v.size(); i++)
    {
        switch (v[i].type)
        {
        case WAIT:
            DoSleep(500);
            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;
            PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
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
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v[i].res));
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
                        return 0;
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
                    DoSleep(35);
                }
            }
            break;

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
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(MyCreateRegion(v[i].res));
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
                        DoSleep(35);
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
                        DoSleep(35);
                    }
                }
                break;
            }
        }
    }

    DoSleep(500);

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
        SelectObject(hdcMem, hbmOld);
    }

    InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    PlaySound(MAKEINTRESOURCE(1000 + g_nMoji), g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
    DoSleep(500);

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
        DestroyMenu(hMenu);
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
    LPCTSTR psz = g_aszMojiReadings[g_nMoji];
    LPCTSTR pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption1, (pch + 1));

    // 意味。
    WCHAR szMeaning[128];
    LoadString(g_hInstance, 2000 + g_nMoji, szMeaning, _countof(szMeaning));
    psz = szMeaning;
    pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption2, (pch + 1));

    g_kanji1_history.insert(nMoji);

    if (g_hbmClient)
        DeleteObject(g_hbmClient);
    g_hbmClient = NULL;
    InvalidateRect(hwnd, NULL, FALSE);

    PlaySound(MAKEINTRESOURCE(3000 + nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

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

BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
    if (codeHitTest != HTCLIENT)
        return FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, msg, DefWindowProc);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);

    RECT rc;
    for (INT j = 0; j < 80; ++j)
    {
        INT ix = j % 10;
        INT iy = j / 10;
        rc.left = ix * (50 + 10) + 5 + 25;
        rc.top = iy * (50 + 10) + 5 + 10;
        if (iy >= 4)
            rc.top += 15;
        rc.right = rc.left + (50 + 10) - 10;
        rc.bottom = rc.top + (50 + 10) - 10;
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

void Caption_OnPaint(HWND hwnd)
{
    TCHAR szText[256];
    GetWindowText(hwnd, szText, 256);
    RECT rc;
    GetClientRect(hwnd, &rc);
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
        smartDrawText(hdc, szText, &rc, 550);
        SelectObject(hdc, hFontOld);
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

    return TRUE;
}

void MoveCaptionWnd(HWND hwnd, HWND hwndCaption, INT nIndex)
{
    RECT rc;
    GetWindowRect(hwnd, &rc);

    TCHAR szText[256];
    GetWindowText(hwndCaption, szText, 256);

    BOOL multiline = FALSE;
    SIZE siz;
    if (HDC hdc = GetDC(hwndCaption))
    {
        HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
        multiline = smartGetTextExtent(hdc, szText, 550, &siz);
        SelectObject(hdc, hFontOld);
        ReleaseDC(hwndCaption, hdc);
    }

    RECT rcNew;
    rcNew.left = (rc.left + rc.right - siz.cx) / 2;
    rcNew.top = (nIndex == 0) ? (rc.top - 10 - siz.cy) : (rc.bottom + 10);
    rcNew.right = rcNew.left + siz.cx;
    rcNew.bottom = rcNew.top + siz.cy;

    DWORD style = GetWindowStyle(hwndCaption);
    DWORD exstyle = GetWindowExStyle(hwndCaption);
    AdjustWindowRectEx(&rcNew, style, FALSE, exstyle);
    MoveWindow(hwndCaption, rcNew.left, rcNew.top, rcNew.right - rcNew.left, rcNew.bottom - rcNew.top, TRUE);
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
    for (i = 0; i < _countof(g_ahbmKanji1); ++i)
    {
        if (g_ahbmKanji1[i])
            DeleteObject(g_ahbmKanji1[i]);
    }

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbrRed);

    g_kanji1_history.clear();

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

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    WCHAR wch;
    TCHAR szText[MAX_PATH], szURL[MAX_PATH];
    tstring str;

    switch (id)
    {
    case 1000:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR psz = szText;
            LPTSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
            str = psz;
            wsprintf(szURL, LoadStringDx(1000), str.c_str());
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;

    case 1001:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR psz = szText;
            LPTSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
#ifdef UNICODE
            wch = *psz;
#else
            MultiByteToWideChar(CP_ACP, 0, psz, -1, sz, 2);
            wch = sz[0];
#endif
            wsprintf(szURL, LoadStringDx(1001), wch);
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    case 1002:
    case 1003:
    case 1004:
    case 1005:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPTSTR psz = szText;
            LPTSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
            wsprintf(szURL, LoadStringDx(id), psz);
            ShellExecute(hwnd, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    }
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
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDBLCLK, OnRButtonDown);
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
        return 1;
    }

    // ウィンドウクラスを登録する。
    WNDCLASSEX wcx = { sizeof(wcx) };
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

    // クライアント領域のサイズとスタイルを元にウィンドウサイズを決める。
    DWORD style = WS_SYSMENU | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX;
    DWORD exstyle = 0;
    RECT rc = { 0, 0, 654, 521 };
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
