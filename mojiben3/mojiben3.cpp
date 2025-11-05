// Moji No Benkyou (3)
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

#include "../CGdiObj.h"
#include "../CDebug.h"
#include "../Common.h"
#include "../MyLib/MyLib.h"

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

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

std::vector<HBITMAP> g_ahbmMoji;
HBITMAP g_hbmClient;
HBITMAP g_hbmKakijun; // Week ref
HBITMAP g_hbmKazoekata;
HBITMAP g_hbmKukuNoUta;

INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;

std::set<INT> g_digits_history;

std::wstring g_section;
MyLib *g_pMyLib = NULL;
MyLibStringTable *g_pMoji = NULL;
BOOL g_fJapanese = FALSE;

static LPCWSTR g_aszReadings[] =
{
    L"0:ゼロ、(れい)",
    L"1:いち",
    L"2:に",
    L"3:さん",
    L"4:よん、(し)",
    L"5:ご",
    L"6:ろく",
    L"7:なな、(しち)",
    L"8:はち",
    L"9:きゅう、(く)",
    L"10:じゅう",
    L"20:にじゅう",
    L"30:さんじゅう",
    L"40:よんじゅう",
    L"50:ごじゅう",
    L"60:ろくじゅう",
    L"70:ななじゅう",
    L"80:はちじゅう",
    L"90:きゅうじゅう",
    L"100:ひゃく",
    L"200:にひゃく",
    L"300:さんびゃく",
    L"400:よんひゃく",
    L"500:ごひゃく",
    L"600:ろっぴゃく",
    L"700:ななひゃく",
    L"800:はっぴゃく",
    L"900:きゅうひゃく",
    L"1000:せん",
    L"2000:にせん",
    L"3000:さんぜん",
    L"4000:よんせん",
    L"5000:ごせん",
    L"6000:ろくせん",
    L"7000:ななせん",
    L"8000:はっせん",
    L"9000:きゅうせん",
    L"10000:いちまん",
    L"20000:にまん",
    L"30000:さんまん",
    L"33421:さんまんさんぜんよん\r\nひゃくにじゅういち",
    L"58698:ごまんはっせんろっ\r\nぴゃくきゅうじゅうはち",
};

static LPCWSTR g_aszKanji[] =
{
    L"0:零",
    L"1:一",
    L"2:二",
    L"3:三",
    L"4:四",
    L"5:五",
    L"6:六",
    L"7:七",
    L"8:八",
    L"9:九",
    L"10:十",
    L"20:二十",
    L"30:三十",
    L"40:四十",
    L"50:五十",
    L"60:六十",
    L"70:七十",
    L"80:八十",
    L"90:九十",
    L"100:百",
    L"200:二百",
    L"300:三百",
    L"400:四百",
    L"500:五百",
    L"600:六百",
    L"700:七百",
    L"800:八百",
    L"900:九百",
    L"1000:千",
    L"2000:二千",
    L"3000:三千",
    L"4000:四千",
    L"5000:五千",
    L"6000:六千",
    L"7000:七千",
    L"8000:八千",
    L"9000:九千",
    L"10000:一万",
    L"20000:二万",
    L"30000:三万",
    L"33421:三万三千四百二十一",
    L"58698:五万八千六百九十八",
};

BOOL g_bHighSpeed = FALSE;

// 「数え方」ボタンの位置。
BOOL GetKazoekataRect(HWND hwnd, LPRECT prc)
{
    BITMAP bm;
    if (!GetObject(g_hbmKazoekata, sizeof(bm), &bm))
        return FALSE;

    RECT rc;
    GetClientRect(hwnd, &rc);

    INT x = (rc.left + rc.right - bm.bmWidth) * 1 / 3;
    INT y = rc.bottom - bm.bmHeight - 20;
    SetRect(prc, x, y, x + bm.bmWidth, y + bm.bmHeight);
    return TRUE;
}

// 「九九の歌」ボタンの位置。
BOOL GetKukuNoUtaRect(HWND hwnd, LPRECT prc)
{
    BITMAP bm;
    if (!GetObject(g_hbmKukuNoUta, sizeof(bm), &bm))
        return FALSE;

    RECT rc;
    GetClientRect(hwnd, &rc);

    INT x = (rc.left + rc.right - bm.bmWidth) * 2 / 3;
    INT y = rc.bottom - bm.bmHeight - 20;
    SetRect(prc, x, y, x + bm.bmWidth, y + bm.bmHeight);
    return TRUE;
}

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
        DWORD size;
        PVOID pres = MyLoadRes(g_hInstance, L"MP3", MAKEINTRESOURCEW(1000 + i), &size);
        std::string binary((char *)pres, size);
        wsprintfW(file, L"%s\\s\\%s.mp3", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(binary, file);
#endif
    }
}

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

    EnumData();

    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    WCHAR file[MAX_PATH];

    wsprintfW(file, L"%s\\00CountingUnits%s.gif", g_section.c_str(), g_fJapanese ? L"_ja" : L"_en");
    g_hbmKazoekata = g_pMyLib->load_picture(file);
    assert(g_hbmKazoekata);

    wsprintfW(file, L"%s\\01MultipicationTable%s.gif", g_section.c_str(), g_fJapanese ? L"_ja" : L"_en");
    g_hbmKukuNoUta = g_pMyLib->load_picture(file);
    assert(g_hbmKukuNoUta);

    updateSystemMenu(hwnd);

    g_hbmClient = NULL;

    INT cx = GetSystemMetrics(SM_CXBORDER);
    INT cy = GetSystemMetrics(SM_CYBORDER);
    g_hKakijunWnd = CreateWindow(g_szKakijunClassName, TEXT(""),
        WS_POPUPWINDOW, CW_USEDEFAULT, 0, 254 + cx * 2, 254 + cy * 2,
        hwnd, NULL, g_hInstance, NULL);
    if (g_hKakijunWnd == NULL)
        return FALSE;

    return TRUE;
}

void OnDestroy(HWND hwnd)
{
    if (g_hThread)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
        CloseHandle(g_hThread);
    }

    for (UINT i = 0; i < g_ahbmMoji.size(); ++i)
        DeleteObject(g_ahbmMoji[i]);
    g_ahbmMoji.clear();

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbmKazoekata);

    DeleteObject(g_hbrRed);

    g_digits_history.clear();

    delete g_pMoji;
    g_pMoji = NULL;

    delete g_pMyLib;
    g_pMyLib = NULL;

    PostQuitMessage(0);
}

// 文字ボタンの位置。
BOOL GetMojiRect(LPRECT prc, INT nMoji)
{
    prc->left = (nMoji % 6) * 80;
    prc->top = 20;
    while (nMoji >= 6)
    {
        prc->top += 40;
        nMoji -= 6;
    }
    prc->left = nMoji * 100 + 20;
    prc->right = prc->left + 80;
    prc->bottom = prc->top + 30;
    return TRUE;
}

VOID OnDraw(HWND hwnd, HDC hdc)
{
    HGDIOBJ hbmOld, hbmOld2;
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

        for (UINT j = 0; j < g_ahbmMoji.size(); ++j)
        {
            GetMojiRect(&rc, j);
            InflateRect(&rc, 3, 3);

            if (g_digits_history.find(j) == g_digits_history.end())
                FillRect(hdcMem2, &rc, GetStockBrush(BLACK_BRUSH));
            else
                FillRect(hdcMem2, &rc, g_hbrRed);

            InflateRect(&rc, -3, -3);
            hbmOld = SelectObject(hdcMem, g_ahbmMoji[j]);
            BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        GetKazoekataRect(hwnd, &rc);
        hbmOld = SelectObject(hdcMem, g_hbmKazoekata);
        BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);

        GetKukuNoUtaRect(hwnd, &rc);
        hbmOld = SelectObject(hdcMem, g_hbmKukuNoUta);
        BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);

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

void PreDraw(HDC hdc, RECT& rc)
{
    std::wstring moji = g_pMoji->key_at(g_nMoji);

    // Load GIF
    WCHAR file[MAX_PATH];
    wsprintfW(file, L"%s\\m\\%s.gif", g_section.c_str(), moji.c_str());
    HBITMAP hbm = g_pMyLib->load_picture(file);
    assert(hbm);

    HDC hdc2 = CreateCompatibleDC(NULL);
    HGDIOBJ hbmOld = SelectObject(hdc2, hbm);
    BitBlt(hdc, rc.left, rc.top, 254, 254, hdc2, 0, 0, SRCCOPY);
    SelectObject(hdc2, hbmOld);
    DeleteDC(hdc2);

    DeleteObject(hbm);
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    LPCWSTR pszText = g_aszReadings[g_nMoji];

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);
        hbm1 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbm2 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);

        hbmOld = SelectObject(hdcMem, hbm1);
        PreDraw(hdcMem, rc);
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
    g_pMyLib->play_sound(stroke_path);

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);
        hbm1 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);
        hbm2 = CreateCompatibleBitmap(hdc, siz.cx, siz.cy);

        hbmOld = SelectObject(hdcMem, hbm1);
        PreDraw(hdcMem, rc);
        SelectObject(hdcMem, hbmOld);
    }

    DO_SLEEP(800);

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

    g_digits_history.insert(nMoji);

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

    // 文字ボタンの当たり判定。
    for (UINT j = 0; j < g_ahbmMoji.size(); ++j)
    {
        GetMojiRect(&rc, j);
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, j, fRight);
            return;
        }
    }

    // 「数え方」ボタンの当たり判定。
    GetKazoekataRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        ShellExecute(hwnd, NULL, LoadStringDx(1000), NULL, NULL, SW_SHOWNORMAL);
    }

    // 「九九の歌」ボタンの当たり判定。
    GetKukuNoUtaRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        ShellExecute(hwnd, NULL, LoadStringDx(1001), NULL, NULL, SW_SHOWNORMAL);
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
    for (UINT j = 0; j < g_ahbmMoji.size(); ++j)
    {
        GetMojiRect(&rc, j);
        if (PtInRect(&rc, pt))
        {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
    }

    GetKazoekataRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    GetKukuNoUtaRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
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
    if (g_hbmKakijun != NULL)
    {
        CDC hdcMem(hdc);
        hbmOld = SelectObject(hdcMem, g_hbmKakijun);
        BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
    }
}

BOOL Kakijun_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    return TRUE;
}

void Kakijun_OnShowWindow(HWND hwnd, BOOL fShow, UINT status)
{
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
}

void Kakijun_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    ShowWindow(hwnd, SW_HIDE);
}

void Kakijun_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (vk == VK_ESCAPE)
        ShowWindow(hwnd, SW_HIDE);
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
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
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
    RECT rc = { 0, 0, 620, 420 };
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
