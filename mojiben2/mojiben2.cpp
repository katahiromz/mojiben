// Moji No Benkyou (2)
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
#include <assert.h>

#include <vector>
#include <map>
#include <set>

#include "../kakijun.h"
#include "../CGdiObj.h"
#include "../CDebug.h"
#include "../Common.h"
#include "../mstr.h"
#include "../MyLib/MyLib.h"

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (2)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (2) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

HBITMAP g_hbmUppercaseON, g_hbmLowercaseON;
HBITMAP g_hbmUppercaseOFF, g_hbmLowercaseOFF;
HBITMAP g_hbmClient;
BOOL g_fLowerCase;

HBITMAP g_hbmKakijun; // Week ref
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;
HPEN g_hPenBlue;

std::set<INT> g_history;

BOOL g_bHighSpeed = FALSE;

std::wstring g_section;
MyLib *g_pMyLib = NULL;
MyLibStringTable *g_pMoji = NULL;
MyLibStringTable *g_pMain = NULL;
HBITMAP g_hbmBack;
std::vector<HBITMAP> g_ahbmMoji;
KAKIJUN g_kakijun;
BOOL g_fJapanese = FALSE;

void GetMojiValues(std::vector<std::wstring>& values, INT j) {
    values.clear();
    std::wstring moji = g_pMoji->key_at(j);
    std::wstring str = (*g_pMain)[moji];
    mstr_split(values, str, L",");
    assert(values.size() == 3);
}

// 文字ボタンの位置。
BOOL GetMojiRect(HWND hwnd, LPRECT prc, INT i)
{
    INT j = 0;
    BITMAP bm;
    GetObjectW(g_ahbmMoji[0], sizeof(bm), &bm);

#if 1
    std::vector<std::wstring> values;
    GetMojiValues(values, i);
    prc->left = _wtoi(values[0].c_str());
    prc->top = _wtoi(values[1].c_str());
#else
    if (i < 14)
    {
        prc->left = i * 54 + 10;
        prc->top = j * 70 + 180;
    }
    else
    {
        j = 1;
        prc->left = (i - 14) * 54 + 10;
        prc->top = j * 70 + 180;
    }
#endif

    prc->right = prc->left + bm.bmWidth;
    prc->bottom = prc->top + bm.bmHeight;

    return TRUE;
}

void EnumData() {
    WCHAR file[MAX_PATH];

    for (size_t i = 0; i < g_pMoji->size(); ++i) {
        std::wstring moji = g_pMoji->key_at(i);

#if 0
        DWORD size;
        PVOID pres;
        if (i < 26)
            pres = MyLoadRes(g_hInstance, L"GIF", MAKEINTRESOURCEW(1000 + i % 26), &size);
        else
            pres = MyLoadRes(g_hInstance, L"GIF", MAKEINTRESOURCEW(2000 + i % 26), &size);
        std::string binary((char *)pres, size);
        if (i < 26)
            wsprintfW(file, L"%s\\i\\U-%s.gif", g_section.c_str(), moji.c_str());
        else
            wsprintfW(file, L"%s\\i\\L-%s.gif", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(binary, file);
#endif

        // Load GIF
        if (i < 26)
            wsprintfW(file, L"%s\\i\\U-%s.gif", g_section.c_str(), moji.c_str());
        else
            wsprintfW(file, L"%s\\i\\L-%s.gif", g_section.c_str(), moji.c_str());
        HBITMAP hbm = g_pMyLib->load_picture(file);
        assert(hbm);
        g_ahbmMoji.push_back(hbm);

#if 0
        if (i < 26) {
            DWORD size;
            PVOID pres = MyLoadRes(g_hInstance, L"MP3", MAKEINTRESOURCEW(5000 + i % 26), &size);
            std::string binary((char *)pres, size);
            wsprintfW(file, L"%s\\s\\%s.mp3", g_section.c_str(), moji.c_str());
            g_pMyLib->save_binary(binary, file);
        }
#endif

#if 0
        {
            RECT rc;
            GetMojiRect(g_hMainWnd, &rc, i % 26);
            WCHAR text[128];
            wsprintfW(text, L"%s = (%d, %d)\n", moji.c_str(), rc.left, rc.top);
            OutputDebugStringW(text);
        }
#endif

#if 0
        VOID InitPrintLowerCase(VOID);
        VOID InitPrintUpperCase(VOID);
        extern std::vector<std::vector<STROKE> > g_print_lowercase_kakijun;
        extern std::vector<std::vector<STROKE> > g_print_uppercase_kakijun;
        InitPrintLowerCase();
        InitPrintUpperCase();
        std::vector<STROKE> v = i < 26 ? g_print_uppercase_kakijun[i % 26] : g_print_lowercase_kakijun[i % 26];
        std::vector<std::string> values;
        char buf[MAX_PATH];
        for (size_t k = 0; k < v.size(); ++k) {
            switch (v[k].type) {
            case STROKE::WAIT:
                values.push_back("W");
                break;
            case STROKE::LINEAR:
                wsprintfA(buf, "L,%d", v[k].angle0);
                values.push_back(buf);
                break;
            case STROKE::DOT:
                wsprintfA(buf, "D");
                values.push_back(buf);
                break;
            case STROKE::POLAR:
                wsprintfA(buf, "P,%d,%d,%d,%d", v[k].angle0, v[k].angle1, v[k].cx, v[k].cy);
                values.push_back(buf);
                break;
            }
        }
        std::string ansi = mstr_join(values, ";");
        if (i < 26)
            wsprintfW(file, L"%s\\kkj\\U-%s.kkj", g_section.c_str(), moji.c_str());
        else
            wsprintfW(file, L"%s\\kkj\\L-%s.kkj", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(ansi, file);
#endif

        {
            std::vector<STROKE> v;
            STROKE stroke;
            if (i < 26)
                wsprintfW(file, L"%s\\kkj\\U-%s.kkj", g_section.c_str(), moji.c_str());
            else
                wsprintfW(file, L"%s\\kkj\\L-%s.kkj", g_section.c_str(), moji.c_str());
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

#if 0
        {
            INT iKakijun = (((i >= 26) ? 100 : 200) + (i % 26)) * 100;
            INT ires = 0;
            for (size_t k = 0; k < g_kakijun[i].size(); ++k) {
                if (g_kakijun[i][k].type != STROKE::WAIT) {
                    DWORD size;
                    PVOID pres = MyLoadRes(g_hInstance, RT_RCDATA, MAKEINTRESOURCEW(iKakijun + ires), &size);
                    std::string binary((char *)pres, size);
                    assert(size);
                    if (i >= 26)
                        wsprintfW(file, L"%s\\kkj\\L-%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)ires);
                    else
                        wsprintfW(file, L"%s\\kkj\\U-%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)ires);
                    g_pMyLib->save_binary(binary, file);
                    ++ires;
                }
            }
        }
#endif
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

    g_pMain = new(std::nothrow) MyLibStringTable();
    if (!g_pMain)
        return FALSE;
    g_pMyLib->load_string_table(*g_pMain, g_section + L"\\Main.txt");

    // 開始時の音を鳴らす。これにより最初の音遅れを回避する。
    std::wstring start_sound = g_pMyLib->find_data_file(g_section + L"\\Start.mp3");
    g_pMyLib->play_sound_async(start_sound);

    EnumData();

    WCHAR file[MAX_PATH];

    wsprintfW(file, L"%s\\%s.gif", g_section.c_str(), L"00UppercaseON");
    g_hbmUppercaseON = g_pMyLib->load_picture(file);
    wsprintfW(file, L"%s\\%s.gif", g_section.c_str(), L"01LowercaseON");
    g_hbmLowercaseON = g_pMyLib->load_picture(file);
    wsprintfW(file, L"%s\\%s.gif", g_section.c_str(), L"02UppercaseOFF");
    g_hbmUppercaseOFF = g_pMyLib->load_picture(file);
    wsprintfW(file, L"%s\\%s.gif", g_section.c_str(), L"03LowercaseOFF");
    g_hbmLowercaseOFF = g_pMyLib->load_picture(file);
    wsprintfW(file, L"%s\\%s.gif", g_section.c_str(), L"Back");
    g_hbmBack = g_pMyLib->load_picture(file);

    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));
    g_hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

    g_fLowerCase = FALSE;

    updateSystemMenu(hwnd);

    g_hbmClient = NULL;

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

    DeleteObject(g_hbmUppercaseON);
    DeleteObject(g_hbmLowercaseON);
    DeleteObject(g_hbmUppercaseOFF);
    DeleteObject(g_hbmLowercaseOFF);

    UINT i;
    for (i = 0; i < g_ahbmMoji.size(); ++i)
    {
        if (g_ahbmMoji[i])
            DeleteObject(g_ahbmMoji[i]);
    }
    g_ahbmMoji.clear();

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmBack);

    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbrRed);
    DeleteObject(g_hPenBlue);

    g_history.clear();

    delete g_pMoji;
    g_pMoji = NULL;

    delete g_pMain;
    g_pMain = NULL;

    delete g_pMyLib;
    g_pMyLib = NULL;

    PostQuitMessage(0);
}

// 「UPPERCASE」ボタンの位置。
BOOL GetUppercaseRect(HWND hwnd, LPRECT prc)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    BITMAP bm;
    GetObjectW(g_hbmUppercaseON, sizeof(bm), &bm);
    INT x = rc.right / 2 - bm.bmWidth - 30;
    SetRect(prc, x, 60, x + bm.bmWidth, 60 + bm.bmHeight);
    return TRUE;
}

// 「lowercase」ボタンの位置。
BOOL GetLowercaseRect(HWND hwnd, LPRECT prc)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    BITMAP bm;
    GetObjectW(g_hbmUppercaseON, sizeof(bm), &bm);
    INT x = rc.right / 2 + 30;
    SetRect(prc, x, 60, x + bm.bmWidth, 60 + bm.bmHeight);
    return TRUE;
}

void OnDraw(HWND hwnd, HDC hdc)
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

        // Draw background
        hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
        if (g_hbmBack) {
            BITMAP bm;
            GetObjectW(g_hbmBack, sizeof(bm), &bm);

            HDC hdcMem3 = CreateCompatibleDC(NULL);
            HGDIOBJ hbmOld = SelectObject(hdcMem3, g_hbmBack);
            for (INT y = 0; y < siz.cy + bm.bmHeight; y += bm.bmHeight) {
                for (INT x = 0; x < siz.cx + bm.bmWidth; x += bm.bmWidth) {
                    BitBlt(hdcMem2, x, y, bm.bmWidth, bm.bmHeight, hdcMem3, 0, 0, SRCCOPY);
                }
            }
            SelectObject(hdcMem3, hbmOld);
            DeleteDC(hdcMem3);
        } else {
            hbr = CreateSolidBrush(RGB(255, 255, 192));
            FillRect(hdcMem2, &rc, hbr);
            DeleteObject(hbr);
        }

        RECT rcU, rcL;
        GetUppercaseRect(hwnd, &rcU);
        GetLowercaseRect(hwnd, &rcL);

        if (g_fLowerCase)
        {
            hbmOld = SelectObject(hdcMem, g_hbmUppercaseOFF);
            BitBlt(hdcMem2, rcU.left, rcU.top, rcU.right - rcU.left, rcU.bottom - rcU.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmLowercaseON);
            BitBlt(hdcMem2, rcL.left, rcL.top, rcL.right - rcL.left, rcL.bottom - rcL.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        else
        {
            hbmOld = SelectObject(hdcMem, g_hbmUppercaseON);
            BitBlt(hdcMem2, rcU.left, rcU.top, rcU.right - rcU.left, rcU.bottom - rcU.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmLowercaseOFF);
            BitBlt(hdcMem2, rcL.left, rcL.top, rcL.right - rcL.left, rcL.bottom - rcL.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        for (UINT i = 0; i < g_ahbmMoji.size(); i++) {
            if (!g_fLowerCase) {
                if (i >= 26)
                    continue;
            } else {
                if (i < 26)
                    continue;
            }

            GetMojiRect(hwnd, &rc, i % 26);

            InflateRect(&rc, +2, +2);
            OffsetRect(&rc, +1, +1);

            hbmOld = SelectObject(hdcMem, g_ahbmMoji[i]);
            if (g_fLowerCase)
            {
                if (g_history.find(i) != g_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else
            {
                if (g_history.find(i) != g_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }

            OffsetRect(&rc, -1, -1);
            InflateRect(&rc, -2, -2);

            BitBlt(hdcMem2, rc.left, rc.top, 48, 48, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        SelectObject(hdcMem2, hbmOld2);
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
}

VOID DrawGuideline(HDC hdcMem, INT cx)
{
#define TRANSLATE_300_TO_254(x) (((x) * 254) / 300)
    MoveToEx(hdcMem, 0, TRANSLATE_300_TO_254(15), NULL);
    LineTo(hdcMem, cx, TRANSLATE_300_TO_254(15));
    MoveToEx(hdcMem, 0, TRANSLATE_300_TO_254(108), NULL);
    LineTo(hdcMem, cx, TRANSLATE_300_TO_254(108));
    MoveToEx(hdcMem, 0, TRANSLATE_300_TO_254(194), NULL);
    LineTo(hdcMem, cx, TRANSLATE_300_TO_254(194));
    MoveToEx(hdcMem, 0, TRANSLATE_300_TO_254(195), NULL);
    LineTo(hdcMem, cx, TRANSLATE_300_TO_254(195));
    MoveToEx(hdcMem, 0, TRANSLATE_300_TO_254(285), NULL);
    LineTo(hdcMem, cx, TRANSLATE_300_TO_254(285));
}

HRGN MyCreateRegion(INT nIndex, INT iKakijun, INT i, INT ires)
{
#if 1
    std::vector<STROKE>& v = g_kakijun[nIndex];
    std::wstring moji = g_pMoji->key_at(nIndex);
    INT k = ires;
    assert(v[i].type != STROKE::WAIT);
    WCHAR file[MAX_PATH];
    if (nIndex < 26)
        wsprintfW(file, L"%s\\kkj\\U-%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)k);
    else
        wsprintfW(file, L"%s\\kkj\\L-%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)k);
    std::string binary;
    g_pMyLib->load_binary(binary, file);
    return DeserializeRegion254((PBYTE)binary.c_str(), (DWORD)binary.size());
#else
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(ires), RT_RCDATA);
    DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    return DeserializeRegion254((PBYTE)pvData, cbData);
#endif
}

void GetStrokeData(std::vector<STROKE>& v)
{
    INT index = g_nMoji;

#if 1
    v = g_kakijun[index % 26 + g_fLowerCase * 26];
#else
    if (g_fLowerCase)
        v = g_print_lowercase_kakijun[g_nMoji % 26];
    else
        v = g_print_uppercase_kakijun[g_nMoji % 26];
#endif
}

void PreDraw(HDC hdc, RECT& rc)
{
    HGDIOBJ hPenOld = SelectObject(hdc, g_hPenBlue);
    DrawGuideline(hdc, rc.right);
    SelectObject(hdc, hPenOld);
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    INT k;
    POINT apt[5];

    INT nIndex = g_nMoji % 26 + g_fLowerCase * 26;
    std::vector<STROKE> v;
    GetStrokeData(v);

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    INT iKakijun = ((g_fLowerCase ? 200 : 100) + g_nMoji) * 100;
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

    std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\s\\" + g_pMoji->key_at(nIndex) + L".mp3");
    g_pMyLib->play_sound(mp3_path);

    if (!IsWindowVisible(g_hKakijunWnd))
        return 0;

    DO_SLEEP(200);

    std::wstring stroke_sound = g_pMyLib->find_data_file(g_section + L"\\Stroke.mp3");
    g_pMyLib->play_sound_async(stroke_sound);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); ++i)
    {
        switch (v[i].type)
        {
        case STROKE::WAIT:
            DO_SLEEP(500);

            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;

            g_pMyLib->play_sound_async(stroke_sound);
            break;

        case STROKE::DOT:
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

    g_pMyLib->play_sound(mp3_path);

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

void OnMojiRightClick(HWND hwnd) {
    MyLibStringTable menu;
    g_pMyLib->load_string_table(menu, g_section + (g_fJapanese ? L"\\MojiMenu_ja.txt" : L"\\MojiMenu_en.txt"));

    std::wstring moji = g_pMoji->key_at(g_nMoji);
    WCHAR hira[64], kata[64], upper[64], lower[64];
    LCMapStringW(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), LCMAP_HIRAGANA, moji.c_str(), -1, hira, _countof(hira));
    LCMapStringW(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), LCMAP_KATAKANA, moji.c_str(), -1, kata, _countof(kata));
    LCMapStringW(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), LCMAP_UPPERCASE, moji.c_str(), -1, upper, _countof(upper));
    LCMapStringW(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), LCMAP_LOWERCASE, moji.c_str(), -1, lower, _countof(lower));

    for (size_t i = 0; i < menu.size(); ++i) {
        mstr_replace(menu.m_pairs[i].m_key, L"<Hiragana>", hira);
        mstr_replace(menu.m_pairs[i].m_key, L"<Katakana>", kata);
        mstr_replace(menu.m_pairs[i].m_key, L"<Uppercase>", upper);
        mstr_replace(menu.m_pairs[i].m_key, L"<Lowercase>", lower);
        mstr_replace(menu.m_pairs[i].m_value, L"<Hiragana>", hira);
        mstr_replace(menu.m_pairs[i].m_value, L"<Katakana>", kata);
        mstr_replace(menu.m_pairs[i].m_value, L"<Uppercase>", upper);
        mstr_replace(menu.m_pairs[i].m_value, L"<Lowercase>", lower);
    }

    HMENU hMenu = CreatePopupMenu();

    for (size_t i = 0; i < menu.size(); ++i) {
        if (menu.key_at(i) == L"---") {
            AppendMenu(hMenu, MF_ENABLED | MF_SEPARATOR, 0, NULL);
            continue;
        }
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, 100 + i, menu.key_at(i).c_str());
    }

    SetForegroundWindow(hwnd);

    POINT pt;
    GetCursorPos(&pt);

    INT nCmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);

    if (nCmd) {
        INT iSelected = nCmd - 100;
        g_history.insert(g_nMoji);

        if (g_hbmClient) {
            DeleteObject(g_hbmClient);
            g_hbmClient = NULL;
        }
        InvalidateRect(hwnd, NULL, TRUE);

        if (menu.value_at(iSelected) == L"OnCopyMoji") {
            CopyText(hwnd, moji.c_str());
            return;
        }

        ShellExecute(hwnd, NULL, menu.value_at(iSelected).c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

VOID MojiOnClick(HWND hwnd, INT nMoji, BOOL fRight)
{
    g_nMoji = nMoji;

    if (fRight)
    {
        OnMojiRightClick(hwnd);
        return;
    }

    RECT rc, rc2;

    GetWindowRect(hwnd, &rc);
    GetWindowRect(g_hKakijunWnd, &rc2);
    MoveWindow(g_hKakijunWnd,
        rc.left + (rc.right - rc.left - (rc2.right - rc2.left)) / 2,
        rc.top + (rc.bottom - rc.top - (rc2.bottom - rc2.top)) / 2,
        rc2.right - rc2.left,
        rc2.bottom - rc2.top,
        TRUE);

    if (g_hbmClient)
        DeleteObject(g_hbmClient);
    g_hbmClient = NULL;
    InvalidateRect(hwnd, NULL, TRUE);

    g_history.insert(nMoji);

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

    // 「UPPERCASE」ボタンの当たり判定。
    GetUppercaseRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        g_fLowerCase = FALSE;

        std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\04Uppercase.mp3");
        g_pMyLib->play_sound_async(mp3_path);

        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }

    // 「lowercase」ボタンの当たり判定。
    GetLowercaseRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        g_fLowerCase = TRUE;
        std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\05Lowercase.mp3");
        g_pMyLib->play_sound_async(mp3_path);
        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, TRUE);
        return;
    }

    // 文字ボタンの当たり判定。
    for (INT i = 0; i < 'Z' - 'A' + 1; ++i)
    {
        GetMojiRect(hwnd, &rc, i);
        if (PtInRect(&rc, pt))
        {
            MojiOnClick(hwnd, i + g_fLowerCase * 26, fRight);
            return;
        }
    }
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

    GetUppercaseRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    GetLowercaseRect(hwnd, &rc);
    if (PtInRect(&rc, pt))
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    for (INT i = 0; i < 'Z' - 'A' + 1; i++)
    {
        GetMojiRect(hwnd, &rc, i);
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

// WM_RBUTTONDOWN
void Kakijun_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    ShowWindow(hwnd, SW_HIDE);
}

// WM_KEYDOWN
void Kakijun_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (vk == VK_ESCAPE)
        ShowWindow(hwnd, SW_HIDE);
}

LRESULT CALLBACK
KakijunWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hwnd, WM_ERASEBKGND, Kakijun_OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, Kakijun_OnPaint);
        HANDLE_MSG(hwnd, WM_KEYDOWN, Kakijun_OnKey);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Kakijun_OnRButtonDown);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// WM_ERASEBKGND
BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
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

// WM_LBUTTONDOWN
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    OnButtonDown(hwnd, x, y, FALSE);
}

// WM_RBUTTONDOWN
void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
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

// ウィンドウプロシージャ。
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
    wcx.hbrBackground   = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
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
