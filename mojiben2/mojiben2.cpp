#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <cstdlib>
#include <process.h>
#include <cmath>

#include <new>
#include <vector>
#include <map>
#include <set>
using namespace std;

#include "kakijun.h"

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (2)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (2) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

HBITMAP g_hbmUpperCase, g_hbmLowerCase;
HBITMAP g_hbmUpperCase2, g_hbmLowerCase2;
HBITMAP g_ahbmPrintUpperCase[26];
HBITMAP g_ahbmPrintLowerCase[26];
HBITMAP g_hbm;
BOOL g_fLowerCase;

HBITMAP g_hbm2;
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;
HPEN g_hPenBlue;

set<INT> g_print_uppercase_history;
set<INT> g_print_lowercase_history;

LPTSTR LoadStringDx(INT ids)
{
    static TCHAR sz[512];
    LoadString(g_hInstance, ids, sz, 512);
    return sz;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    INT i;
    HMENU hSysMenu;
    MENUITEMINFO mii;

    g_hThread = NULL;
    g_hbm2 = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));
    g_hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

    g_hbmUpperCase = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100));
    g_hbmLowerCase = LoadBitmap(g_hInstance, MAKEINTRESOURCE(150));
    g_hbmUpperCase2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(300));
    g_hbmLowerCase2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(350));

    g_fLowerCase = FALSE;

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

    for(i = 0; i < 26; i++)
    {
        g_ahbmPrintUpperCase[i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + i));
        if (g_ahbmPrintUpperCase[i] == NULL)
            return FALSE;
    }
    for(i = 0; i < 26; i++)
    {
        g_ahbmPrintLowerCase[i] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(2000 + i));
        if (g_ahbmPrintLowerCase[i] == NULL)
            return FALSE;
    }

    g_hbm = NULL;

    try
    {
        InitPrintUpperCase();
        InitPrintLowerCase();
    }
    catch(bad_alloc)
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

void OnDraw(HWND hwnd, HDC hdc)
{
    HDC hdcMem, hdcMem2;
    HGDIOBJ hbmOld, hbmOld2;
    INT i, j;
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

        HBITMAP *bitmaps;
        if (g_fLowerCase)
        {
            hbmOld = SelectObject(hdcMem, g_hbmUpperCase2);
            BitBlt(hdcMem2, 160, 60, 200, 63, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmLowerCase);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 60, 200, 63, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            bitmaps = g_ahbmPrintLowerCase;
        }
        else
        {
            hbmOld = SelectObject(hdcMem, g_hbmUpperCase);
            BitBlt(hdcMem2, 160, 60, 200, 63, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmLowerCase2);
            BitBlt(hdcMem2, siz.cx - (160 + 200), 60, 200, 63, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            bitmaps = g_ahbmPrintUpperCase;
        }

        j = 0;
        for(i = 0; i < 14; i++)
        {
            hbmOld = SelectObject(hdcMem, bitmaps[i]);
                rc.left = i * 54 + 10 - 1;
                rc.top = j * 70 + 180 - 1;
                rc.right = rc.left + 50 + 2;
                rc.bottom = rc.top + 50 + 2;
            if (g_fLowerCase)
            {
                if (g_print_lowercase_history.find(i) != g_print_lowercase_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else
            {
                if (g_print_uppercase_history.find(i) != g_print_uppercase_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            BitBlt(hdcMem2, i * 54 + 11, j * 70 + 181, 48, 48, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        j = 1;
        for(i = 14; i < 26; i++)
        {
            hbmOld = SelectObject(hdcMem, bitmaps[i]);
                rc.left = (i - 14) * 54 + 10 - 1;
                rc.top = j * 70 + 180 - 1;
                rc.right = rc.left + 50 + 2;
                rc.bottom = rc.top + 50 + 2;
            if (g_fLowerCase)
            {
                if (g_print_lowercase_history.find(i) != g_print_lowercase_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else
            {
                if (g_print_uppercase_history.find(i) != g_print_uppercase_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            BitBlt(hdcMem2, (i - 14) * 54 + 11, j * 70 + 181, 48, 48, hdcMem, 0, 0, SRCCOPY);
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

VOID DrawGuideline(HDC hdcMem, INT cx)
{
    MoveToEx(hdcMem, 0, 15, NULL);
    LineTo(hdcMem, cx, 15);
    MoveToEx(hdcMem, 0, 108, NULL);
    LineTo(hdcMem, cx, 108);
    MoveToEx(hdcMem, 0, 194, NULL);
    LineTo(hdcMem, cx, 194);
    MoveToEx(hdcMem, 0, 195, NULL);
    LineTo(hdcMem, cx, 195);
    MoveToEx(hdcMem, 0, 285, NULL);
    LineTo(hdcMem, cx, 285);
}

unsigned __stdcall ThreadProc( void * )
{
    RECT rc;
    SIZE siz;
    HDC hdc, hdcMem;
    HBITMAP hbm, hbm2, hbmTemp;
    HGDIOBJ hbmOld, hPenOld;
    HRGN hRgn, hRgn2, hRgn3, hRgn4, hRgn5;
    vector<GA> v;
    INT k;
    POINT apt[4];
    double cost, sint, cost2, sint2;
    
    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    if (g_fLowerCase)
        v = g_print_lowercase_kakijun[g_nMoji];
    else
        v = g_print_uppercase_kakijun[g_nMoji];

    hRgn = CreateRectRgn(0, 0, 0, 0);
    for(UINT i = 0; i < v.size(); i++)
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

    hPenOld = SelectObject(hdcMem, g_hPenBlue);
    DrawGuideline(hdcMem, siz.cx);
    SelectObject(hdcMem, hPenOld);

    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(g_hKakijunWnd, hdc);

    g_hbm2 = hbm;

    InvalidateRect(g_hKakijunWnd, NULL, TRUE);
    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);
    Sleep(700);

    hRgn5 = CreateRectRgn(0, 0, 0, 0);
    PlaySound(MAKEINTRESOURCE(400), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    for(UINT i = 0; i < v.size(); i++)
    {
        switch(v[i].type)
        {
        case WAIT:
            Sleep(500);
            PlaySound(MAKEINTRESOURCE(400), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
            break;

        case DOT:
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

            hPenOld = SelectObject(hdcMem, g_hPenBlue);
            DrawGuideline(hdcMem, siz.cx);
            SelectObject(hdcMem, hPenOld);

            FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            CombineRgn(hRgn5, hRgn5, hRgn2, RGN_OR);
            FillRgn(hdcMem, hRgn5, g_hbrRed);
            SelectObject(hdcMem, hbmOld);

            DeleteObject(hRgn2);
            DeleteDC(hdcMem);
            ReleaseDC(g_hKakijunWnd, hdc);
            InvalidateRect(g_hKakijunWnd, NULL, TRUE);
            Sleep(50);
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

            hPenOld = SelectObject(hdcMem, g_hPenBlue);
            DrawGuideline(hdcMem, siz.cx);
            SelectObject(hdcMem, hPenOld);

            FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SelectObject(hdcMem, hbmOld);

            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            cost = cos(v[i].angle0 * M_PI / 180);
            sint = sin(v[i].angle0 * M_PI / 180);
            for(k = -200; k < 200; k += 30)
            {
                apt[0].x = LONG(150 + k * cost + 150 * sint);
                apt[0].y = LONG(150 + k * sint - 150 * cost);
                apt[1].x = LONG(150 + k * cost - 150 * sint);
                apt[1].y = LONG(150 + k * sint + 150 * cost);
                apt[2].x = LONG(150 + (k + 30) * cost - 150 * sint);
                apt[2].y = LONG(150 + (k + 30) * sint + 150 * cost);
                apt[3].x = LONG(150 + (k + 30) * cost + 150 * sint);
                apt[3].y = LONG(150 + (k + 30) * sint - 150 * cost);
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
            for( ; k < 200; k += 30)
            {
                hbmTemp = hbm;
                hbm = hbm2;
                hbm2 = hbmTemp;
                g_hbm2 = hbm;
                hbmOld = SelectObject(hdcMem, hbm);
                
                hPenOld = SelectObject(hdcMem, g_hPenBlue);
                DrawGuideline(hdcMem, siz.cx);
                SelectObject(hdcMem, hPenOld);

                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

                apt[0].x = LONG(150 + k * cost + 150 * sint);
                apt[0].y = LONG(150 + k * sint - 150 * cost);
                apt[1].x = LONG(150 + k * cost - 150 * sint);
                apt[1].y = LONG(150 + k * sint + 150 * cost);
                apt[2].x = LONG(150 + (k + 30) * cost - 150 * sint);
                apt[2].y = LONG(150 + (k + 30) * sint + 150 * cost);
                apt[3].x = LONG(150 + (k + 30) * cost + 150 * sint);
                apt[3].y = LONG(150 + (k + 30) * sint - 150 * cost);
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

                InvalidateRect(g_hKakijunWnd, NULL, TRUE);
                if (n == NULLREGION)
                    break;
                Sleep(30);
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

            hPenOld = SelectObject(hdcMem, g_hPenBlue);
            DrawGuideline(hdcMem, siz.cx);
            SelectObject(hdcMem, hPenOld);
            
            FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SelectObject(hdcMem, hbmOld);

            hRgn2 = ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb);
            if (v[i].angle0 <= v[i].angle1)
            {
                for(k = v[i].angle0; k < v[i].angle1; k += 20)
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

                    hPenOld = SelectObject(hdcMem, g_hPenBlue);
                    DrawGuideline(hdcMem, siz.cx);
                    SelectObject(hdcMem, hPenOld);

                    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

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

                    InvalidateRect(g_hKakijunWnd, NULL, TRUE);
                    if (n == NULLREGION)
                        break;
                    Sleep(30);
                }
            }
            else
            {
                for(k = v[i].angle0; k > v[i].angle1; k -= 20)
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

                    hPenOld = SelectObject(hdcMem, g_hPenBlue);
                    DrawGuideline(hdcMem, siz.cx);
                    SelectObject(hdcMem, hPenOld);

                    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

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

                    InvalidateRect(g_hKakijunWnd, NULL, TRUE);
                    if (n == NULLREGION)
                        break;
                    Sleep(30);
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
    PlaySound(MAKEINTRESOURCE(5000 + g_nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

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

    hPenOld = SelectObject(hdcMem, g_hPenBlue);
    DrawGuideline(hdcMem, siz.cx);
    SelectObject(hdcMem, hPenOld);

    FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(g_hKakijunWnd, hdc);

    InvalidateRect(g_hKakijunWnd, NULL, TRUE);
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
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, 100 + nMoji, LoadStringDx(100 + g_nMoji));
        SetForegroundWindow(hwnd);

        POINT pt;
        GetCursorPos(&pt);

        INT nCmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  pt.x, pt.y, 0, hwnd, NULL);
        SendMessage(hwnd, WM_COMMAND, nCmd, 0);
        DestroyMenu(hMenu);
        return;
    }

    if (g_hbm != NULL)
        DeleteObject(g_hbm);
    g_hbm = NULL;
    InvalidateRect(hwnd, NULL, TRUE);

    if (g_fLowerCase)
        g_print_lowercase_history.insert(nMoji);
    else
        g_print_uppercase_history.insert(nMoji);
    
    PlaySound(MAKEINTRESOURCE(5000 + nMoji), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
    if (g_hThread != NULL)
    {
        TerminateThread(g_hThread, 0);
        CloseHandle(g_hThread);
    }
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

    rc.left = 160;
    rc.top = 60;
    rc.right = rc.left + 200;
    rc.bottom = rc.top + 63;
    if (PtInRect(&rc, pt))
    {
        g_fLowerCase = FALSE;
        PlaySound(MAKEINTRESOURCE(300), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        if (g_hbm != NULL)
            DeleteObject(g_hbm);
        g_hbm = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }

    rc.left = siz.cx - (160 + 200);
    rc.top = 60;
    rc.right = rc.left + 200;
    rc.bottom = rc.top + 63;
    if (PtInRect(&rc, pt))
    {
        g_fLowerCase = TRUE;
        PlaySound(MAKEINTRESOURCE(301), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        if (g_hbm != NULL)
            DeleteObject(g_hbm);
        g_hbm = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }

    j = 0;
    for(i = 0; i < 14; i++)
    {
        rc.left = i * 54 + 10;
        rc.top = j * 70 + 180;
        rc.right = rc.left + 50;
        rc.bottom = rc.top + 50;
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, i, fRight);
            return;
        }
    }
    j = 1;
    for(i = 14; i < 26; i++)
    {
        rc.left = (i - 14) * 54 + 10;
        rc.top = j * 70 + 180;
        rc.right = rc.left + 50;
        rc.bottom = rc.top + 50;
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, i, fRight);
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

LRESULT CALLBACK
KakijunWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hwnd, WM_ERASEBKGND, Kakijun_OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, Kakijun_OnPaint);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK
AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
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
    if (g_hThread != NULL)
    {
        TerminateThread(g_hThread, 0);
        CloseHandle(g_hThread);
    }
    PostQuitMessage(0);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (id == 0)
        return;

    LPTSTR psz = LoadStringDx(200 + g_nMoji);
    if (psz[0])
        ShellExecute(hwnd, NULL, psz, NULL, NULL, SW_SHOWNORMAL);
}

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
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
    MSG msg;
    BOOL f;

    RECT rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
    INT cxWork = (rcWorkArea.right - rcWorkArea.left);
    INT cyWork = (rcWorkArea.bottom - rcWorkArea.top);
    if (cxWork <= 780 || cyWork <= 430)
    {
        MessageBox(NULL, LoadStringDx(4), NULL, MB_ICONERROR);
        return 1;
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
    wcx.hbrBackground   = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
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
