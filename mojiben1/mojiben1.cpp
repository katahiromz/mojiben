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

#include <new>
#include <vector>
#include <map>
#include <set>
#include <cassert>

#include "../kakijun.h"
#include "../CGdiObj.h"
#include "../CDebug.h"
#include "../Common.h"
#include "../MyLib/MyLib.h"
#include "../mstr.h"

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (1)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (1) Kakijun");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;

HBITMAP g_hbmHiraganaON, g_hbmHiraganaOFF;
HBITMAP g_hbmKatakanaON, g_hbmKatakanaOFF;
HBITMAP g_hbmBack;
std::vector<HBITMAP> g_ahbmMoji;
HBITMAP g_hbmClient;
BOOL g_fKatakana;

HBITMAP g_hbmKakijun = NULL; // Week ref
INT g_nMoji;
HANDLE g_hThread = NULL;
HBRUSH g_hbrRed = NULL;
HFONT g_hFont = NULL;

std::set<INT> g_history;

BOOL g_bHighSpeed = FALSE;

std::wstring g_section;
MyLib *g_pMyLib = NULL;
MyLibStringTable *g_pMoji = NULL;
MyLibStringTable *g_pMain = NULL;
MyLibStringTable *g_pRomaji = NULL;
KAKIJUN g_kakijun;
BOOL g_fJapanese = FALSE;

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

INT MojiIndexFromMojiID(INT moji_id)
{
    if (moji_id >= (INT)g_pMoji->size())
        return -1;
    return moji_id;
}

void EnumData() {
    WCHAR file[MAX_PATH];

    for (size_t i = 0; i < g_pMoji->size(); ++i) {
        std::wstring moji = g_pMoji->key_at(i);

#if 0
        DWORD size;
        PVOID pres = MyLoadRes(g_hInstance, L"GIF", MAKEINTRESOURCEW(g_moji_data[i].bitmap_id), &size);
        std::string binary((char *)pres, size);
        wnsprintfW(file, _countof(file9, L"%s\\i\\%s.gif", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(binary, file);
#endif

        // Load GIF
        wnsprintfW(file, _countof(file), L"%s\\i\\%s.gif", g_section.c_str(), moji.c_str());
        HBITMAP hbm = g_pMyLib->load_picture(file);
        assert(hbm);
        g_ahbmMoji.push_back(hbm);

#if 0
        DWORD size;
        PVOID pres = MyLoadRes(g_hInstance, L"MP3", MAKEINTRESOURCEW(3000 + g_moji_data[i].moji_id), &size);
        std::string binary((char *)pres, size);
        wnsprintfW(file, _countof(file), L"%s\\s\\%s.mp3", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(binary, file);
#endif

#if 0
        {
            INT x = g_moji_data[i].x, y = g_moji_data[i].y;
            WCHAR text[128];
            wnsprintfW(text, _countof(text), L"%s = (%d, %d)\n", moji.c_str(), x, y);
            OutputDebugStringW(text);
        }
#endif

#if 0
        std::vector<STROKE> v = i < 46 ? g_hiragana_kakijun[i % 46] : g_katakana_kakijun[i % 46];
        std::vector<std::string> values;
        char buf[MAX_PATH];
        for (size_t i = 0; i < v.size(); ++i) {
            switch (v[i].type) {
            case STROKE::WAIT:
                values.push_back("W");
                break;
            case STROKE::LINEAR:
                wnsprintfA(buf, _countof(buf), "L,%d", v[i].angle0);
                values.push_back(buf);
                break;
            case STROKE::DOT:
                wnsprintfA(buf, _countof(buf), "D");
                values.push_back(buf);
                break;
            case STROKE::POLAR:
                wnsprintfA(buf, _countof(buf), "P,%d,%d,%d,%d", v[i].angle0, v[i].angle1, v[i].cx, v[i].cy);
                values.push_back(buf);
                break;
            }
        }
        std::string ansi = mstr_join(values, ";");
        wnsprintfW(file, _countof(file), L"%s\\kkj\\%s.kkj", g_section.c_str(), moji.c_str());
        g_pMyLib->save_binary(ansi, file);
#endif

        {
            std::vector<STROKE> v;
            STROKE stroke;
            wnsprintfW(file, _countof(file), L"%s\\kkj\\%s.kkj", g_section.c_str(), moji.c_str());
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
            INT iKakijun = (((i >= 46) ? 200 : 100) + (i % 46)) * 100;
            INT ires = 0;
            for (size_t k = 0; k < g_kakijun[i].size(); ++k) {
                if (g_kakijun[i][k].type != STROKE::WAIT) {
                    DWORD size;
                    PVOID pres = MyLoadRes(g_hInstance, RT_RCDATA, MAKEINTRESOURCEW(iKakijun + ires), &size);
                    std::string binary((char *)pres, size);
                    assert(size);
                    wnsprintfW(file, _countof(file), L"%s\\kkj\\%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)ires);
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

    g_pRomaji = new(std::nothrow) MyLibStringTable();
    if (!g_pRomaji)
        return FALSE;
    g_pMyLib->load_string_table(*g_pRomaji, g_section + L"\\Romaji.txt");

    // 開始時の音を鳴らす。これにより最初の音遅れを回避する。
    std::wstring start_sound = g_pMyLib->find_data_file(g_section + L"\\Start.mp3");
    g_pMyLib->play_sound_async(start_sound);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -20;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfQuality = PROOF_QUALITY;
    lstrcpy(lf.lfFaceName, TEXT("Tahoma"));
    g_hFont = ::CreateFontIndirect(&lf);

    g_hbmClient = NULL;

    EnumData();

    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    WCHAR file[MAX_PATH];

    wnsprintfW(file, _countof(file), L"%s\\%s.gif", g_section.c_str(), L"00ひらがなON");
    g_hbmHiraganaON = g_pMyLib->load_picture(file);
    wnsprintfW(file, _countof(file), L"%s\\%s.gif", g_section.c_str(), L"01ひらがなOFF");
    g_hbmHiraganaOFF = g_pMyLib->load_picture(file);

    wnsprintfW(file, _countof(file), L"%s\\%s.gif", g_section.c_str(), L"02カタカナON");
    g_hbmKatakanaON = g_pMyLib->load_picture(file);
    wnsprintfW(file, _countof(file), L"%s\\%s.gif", g_section.c_str(), L"03カタカナOFF");
    g_hbmKatakanaOFF = g_pMyLib->load_picture(file);

    wnsprintfW(file, _countof(file), L"%s\\%s.gif", g_section.c_str(), L"bg");
    g_hbmBack = g_pMyLib->load_picture(file);

    g_fKatakana = FALSE;

    INT cx = GetSystemMetrics(SM_CXBORDER);
    INT cy = GetSystemMetrics(SM_CYBORDER);
    g_hKakijunWnd = CreateWindow(g_szKakijunClassName, TEXT(""),
        WS_POPUPWINDOW, CW_USEDEFAULT, 0, KAKIJUN_SIZE + cx * 2, KAKIJUN_SIZE + cy * 2,
        hwnd, NULL, g_hInstance, NULL);
    if (g_hKakijunWnd == NULL)
        return FALSE;

    for (size_t i = 0; i < g_pMoji->size(); ++i) {
        std::wstring moji = g_pMoji->key_at(i);
        if (recall_moji(moji)) {
            g_history.insert(i);
        }
    }

    g_bHighSpeed = recall_moji(L"g_bHighSpeed");

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

    for (UINT i = 0; i < g_ahbmMoji.size(); ++i)
    {
        if (g_ahbmMoji[i])
            DeleteObject(g_ahbmMoji[i]);
    }
    g_ahbmMoji.clear();

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmKakijun);

    DeleteObject(g_hFont);
    DeleteObject(g_hbrRed);
    DeleteObject(g_hbmHiraganaON);
    DeleteObject(g_hbmHiraganaOFF);
    DeleteObject(g_hbmKatakanaON);
    DeleteObject(g_hbmKatakanaOFF);
    DeleteObject(g_hbmBack);

    g_history.clear();

    delete g_pMoji;
    g_pMoji = NULL;

    delete g_pMain;
    g_pMain = NULL;

    delete g_pRomaji;
    g_pRomaji = NULL;

    delete g_pMyLib;
    g_pMyLib = NULL;

    PostQuitMessage(0);
}

void GetHiraganaRect(HWND hwnd, PRECT prc)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    BITMAP bm;
    GetObjectW(g_hbmHiraganaON, sizeof(bm), &bm);
    INT x = rc.right / 2 - bm.bmWidth - 30;
    SetRect(prc, x, 10, x + bm.bmWidth, 10 + bm.bmHeight);
}

void GetKatakanaRect(HWND hwnd, PRECT prc)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    BITMAP bm;
    GetObjectW(g_hbmKatakanaON, sizeof(bm), &bm);
    INT x = rc.right / 2 + 30;
    SetRect(prc, x, 10, x + bm.bmWidth, 10 + bm.bmHeight);
}

void GetMojiValues(std::vector<std::wstring>& values, INT j) {
    values.clear();
    std::wstring moji = g_pMoji->key_at(j);
    std::wstring str = (*g_pMain)[moji];
    mstr_split(values, str, L",");
    assert(values.size() == 3);
}

void GetMojiRect(HWND hwnd, PRECT prc, INT j) {
    BITMAP bm;
    GetObjectW(g_ahbmMoji[0], sizeof(bm), &bm);

    std::vector<std::wstring> values;
    GetMojiValues(values, j);

    prc->left = _wtoi(values[0].c_str());
    prc->top = _wtoi(values[1].c_str());
    prc->right = prc->left + bm.bmWidth;
    prc->bottom = prc->top + bm.bmHeight;
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

        RECT rcHira, rcKata;
        GetHiraganaRect(hwnd, &rcHira);
        GetKatakanaRect(hwnd, &rcKata);

        if (g_fKatakana)
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiraganaOFF);
            BitBlt(hdcMem2, rcHira.left, rcHira.top, rcHira.right - rcHira.left, rcHira.bottom - rcHira.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakanaON);
            BitBlt(hdcMem2, rcKata.left, rcKata.top, rcKata.right - rcKata.left, rcKata.bottom - rcKata.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
        else
        {
            hbmOld = SelectObject(hdcMem, g_hbmHiraganaON);
            BitBlt(hdcMem2, rcHira.left, rcHira.top, rcHira.right - rcHira.left, rcHira.bottom - rcHira.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            hbmOld = SelectObject(hdcMem, g_hbmKatakanaOFF);
            BitBlt(hdcMem2, rcKata.left, rcKata.top, rcKata.right - rcKata.left, rcKata.bottom - rcKata.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }

        for (j = 0; j < (INT)g_pMoji->size(); ++j)
        {
            if ((j < (INT)g_pMoji->size() / 2) == g_fKatakana)
                continue;

            hbmOld = SelectObject(hdcMem, g_ahbmMoji[j]);
            GetMojiRect(hwnd, &rc, j);
            InflateRect(&rc, +3, +3);
            if (g_fKatakana)
            {
                if (g_history.find(j) != g_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else
            {
                if (g_history.find(j) != g_history.end())
                    FillRect(hdcMem2, &rc, g_hbrRed);
                else
                    FillRect(hdcMem2, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            InflateRect(&rc, -3, -3);
            BitBlt(hdcMem2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
        }
    }

    hbmOld2 = SelectObject(hdcMem2, g_hbmClient);
    BitBlt(hdc, 0, 0, siz.cx, siz.cy, hdcMem2, 0, 0, SRCCOPY);
    SelectObject(hdcMem2, hbmOld2);
}

HRGN MyCreateRegion(INT nIndex, INT iKakijun, INT i, INT ires) {
#if 1
    std::vector<STROKE>& v = g_kakijun[nIndex];
    std::wstring moji = g_pMoji->key_at(nIndex);
    INT k = ires;
    assert(v[i].type != STROKE::WAIT);
    WCHAR file[MAX_PATH];
    wnsprintfW(file, _countof(file), L"%s\\kkj\\%s-%02d.rgn", g_section.c_str(), moji.c_str(), (int)k);
    std::string binary;
    g_pMyLib->load_binary(binary, file);
    return DeserializeRegion254((PBYTE)binary.c_str(), (DWORD)binary.size());
#else
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(iKakijun + ires), RT_RCDATA);
    DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    return DeserializeRegion254((PBYTE)pvData, cbData);
#endif
}

std::wstring GetStrokeData(std::vector<STROKE>& v) {
    INT index = MojiIndexFromMojiID(g_nMoji);

    v = g_kakijun[(index % 46) + g_fKatakana * 46];

    std::wstring moji = g_pMoji->key_at(index);
    return (*g_pRomaji)[moji];
}

void PreDraw(HDC hdc, RECT& rc, std::wstring romaji)
{
    HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, OPAQUE);
    DrawTextW(hdc, romaji.c_str(), (INT)romaji.size(), &rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
    SelectObject(hdc, hFontOld);
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    INT k;
    POINT apt[5];

    INT nIndex = MojiIndexFromMojiID(g_nMoji);
    std::vector<STROKE> v;
    std::wstring romaji = GetStrokeData(v);

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    CRgn hRgn(::CreateRectRgn(0, 0, 0, 0));
    INT iKakijun = ((g_fKatakana ? 200 : 100) + nIndex) * 100;
    INT ires = 0;
    for (UINT i = 0; i < v.size(); i++)
    {
        if (v[i].type != STROKE::WAIT) {
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
        PreDraw(hdcMem, rc, romaji);
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
                PreDraw(hdcMem, rc, romaji);
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
                PreDraw(hdcMem, rc, romaji);
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
        PreDraw(hdcMem, rc, romaji);
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

void OnMojiRightClick(HWND hwnd, const std::wstring& menu_file) {
    MyLibStringTable menu;
    g_pMyLib->load_string_table(menu, menu_file);

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

        std::wstring value = menu.value_at(i);
        if (value == L"OnHighSpeed") {
            CheckMenuItem(hMenu, 100 + i, (g_bHighSpeed ? MF_CHECKED : MF_UNCHECKED));
            continue;
        }
        if (value == L"OnStudyUsingEnglish") {
            CheckMenuItem(hMenu, 100 + i, ((getStudyModeReal() == STUDY_MODE_USING_ENGLISH) ? MF_CHECKED : MF_UNCHECKED));
            continue;
        }
        if (value == L"OnStudyUsingJapanese") {
            CheckMenuItem(hMenu, 100 + i, ((getStudyModeReal() == STUDY_MODE_USING_JAPANESE) ? MF_CHECKED : MF_UNCHECKED));
            continue;
        }
    }

    SetForegroundWindow(hwnd);

    POINT pt;
    GetCursorPos(&pt);

    INT nCmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);

    if (nCmd) {
        INT iSelected = nCmd - 100;

        if (g_hbmClient) {
            DeleteObject(g_hbmClient);
            g_hbmClient = NULL;
        }
        InvalidateRect(hwnd, NULL, TRUE);

        if (menu.value_at(iSelected) == L"OnCopyMoji") {
            CopyText(hwnd, moji.c_str());
            g_history.insert(g_nMoji);
            remember_moji(g_pMoji->key_at(g_nMoji));
            return;
        }
        if (menu.value_at(iSelected) == L"OnResetAll") {
            for (size_t i = 0; i < g_pMoji->size(); ++i) {
                forget_moji(g_pMoji->key_at(i));
            }
            g_history.clear();
            return;
        }
        if (menu.value_at(iSelected) == L"OnHighSpeed") {
            g_bHighSpeed = !g_bHighSpeed;
            if (g_bHighSpeed)
                remember_moji(L"g_bHighSpeed");
            else
                forget_moji(L"g_bHighSpeed");
            return;
        }
        if (menu.value_at(iSelected) == L"OnAbout") {
            DialogBox(g_hInstance, MAKEINTRESOURCE(1), hwnd, AboutDialogProc);
            return;
        }
        if (menu.value_at(iSelected) == L"OnStudyUsingEnglish") {
            rememberStudyMode(hwnd, STUDY_MODE_USING_ENGLISH);
            return;
        }
        if (menu.value_at(iSelected) == L"OnStudyUsingJapanese") {
            rememberStudyMode(hwnd, STUDY_MODE_USING_JAPANESE);
            return;
        }

        g_history.insert(g_nMoji);
        remember_moji(g_pMoji->key_at(g_nMoji));

        ShellExecute(hwnd, NULL, menu.value_at(iSelected).c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

VOID MojiOnClick(HWND hwnd, INT nMoji, BOOL fRight)
{
    g_nMoji = nMoji;

    if (fRight)
    {
        OnMojiRightClick(hwnd, g_section + (g_fJapanese ? L"\\MojiMenu_ja.txt" : L"\\MojiMenu_en.txt"));
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
    INT nIndex = MojiIndexFromMojiID(g_nMoji);

    g_history.insert(nIndex);
    remember_moji(g_pMoji->key_at(nIndex));

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
    GetHiraganaRect(hwnd, prc);
    return PtInRect(prc, pt);
}

// 「カタカナ」ボタンの当たり判定。
BOOL HitKatakanaRect(HWND hwnd, LPRECT prc, POINT pt)
{
    GetKatakanaRect(hwnd, prc);
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

        std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\04ひらがな.mp3");
        g_pMyLib->play_sound_async(mp3_path);

        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    if (HitKatakanaRect(hwnd, &rc, pt))
    {
        g_fKatakana = TRUE;

        std::wstring mp3_path = g_pMyLib->find_data_file(g_section + L"\\05カタカナ.mp3");
        g_pMyLib->play_sound_async(mp3_path);

        if (g_hbmClient)
            DeleteObject(g_hbmClient);
        g_hbmClient = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    // 文字ボタンの当たり判定。
    for (UINT j = 0; j < g_pMoji->size(); ++j) {
        if (j >= g_pMoji->size() / 2)
            continue;
        GetMojiRect(hwnd, &rc, j);
        InflateRect(&rc, +3, +3);
        if (PtInRect(&rc, pt)) {
            MojiOnClick(hwnd, j + g_fKatakana * 46, fRight);
            return;
        }
        InflateRect(&rc, -3, -3);
    }

    if (fRight) {
        OnMojiRightClick(hwnd, g_section + (g_fJapanese ? L"\\BgMenu_ja.txt" : L"\\BgMenu_en.txt"));
        return;
    }
}

// WM_SETCURSOR
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
    for (UINT j = 0; j < g_pMoji->size(); ++j)
    {
        GetMojiRect(hwnd, &rc, j);
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

// WM_ERASEBKGND
inline BOOL Kakijun_OnEraseBkgnd(HWND hwnd, HDC hdc)
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
    switch (uMsg)
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
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
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
        delete g_pMyLib;
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
