// Moji No Benkyo (4)
// Copyright (C) 2020-2025 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.
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

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (5)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (5) Kakijun");
static const TCHAR g_szCaptionClassName[] = TEXT("Moji No Benkyou (5) Caption");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;

HBITMAP g_ahbmKanji2[160];
HBITMAP g_hbm;

HBITMAP g_hbm2;
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;

std::set<INT> g_kanji2_history;

static const LPCWSTR g_aszMojiReadings[] =
{
    L"引:ひ-く、イン",
    L"羽:はね、は、ウ",
    L"雲:くも、ウン",
    L"園:エン、その",
    L"遠:とお-い、エン",
    L"何:なに、なん、カ",
    L"科:カ",
    L"夏:なつ、カ、ゲ",
    L"家:いえ、や、カ、ケ",
    L"歌:うた、うた-う、カ",
    L"画:ガ、カク”，
    L"回:まわ-る、まわ-す、カイ",
    L"会:あ-う、カイ、エ",
    L"海:うみ、カイ",
    L"絵:エ、カイ",
    L"外:そと、はず-す、はず-れる、ほか、ガイ、ゲ",
    L"角:かど、つの、カク",
    L"楽:ラク、ガク、たの-しい、たの-しむ",
    L"活:カツ",
    L"間:あいだ、ま、カン、ケン",
    L"丸:まる、まる-い、まる-める、ガン",
    L"岩:いわ、ガン",
    L"顔:かお、ガン",
    L"汽:キ、オチ",
    L"記:しる-す、キ",
    L"帰:かえ-る、かえ-す、キ",
    L"弓:ゆみ、キュウ",
    L"牛:うし、ギュウ",
    L"魚:さかな、うお、ギョ",
    L"京:キョウ、ケイ",
    L"強:つよ-い、つよ-まる、つよ-める、キョウ、し-いる、ゴウ",
    L"教:おし-える、おそ-わる、キョウ",
    L"近:ちか-い、キン",
    L"兄:あに、キョウ、ケイ",
    L"形:かたち、かた、ケイ、ギョウ",
    L"計:ケイ、はか-る、はか-らう",
    L"元:もと、ゲン、ガン",
    L"言:いう、ゲン、こと、ゴン",
    L"原:はら、ゲン",
    L"戸:と、コ",
    L"古:ふる-い、コ",
    L"午:ゴ",
    L"後:あと、うしろ、のち、ゴ、コウ",
    L"語:ゴ、かた-らう、かた-る",
    L"工:コウ、ク",
    L"公:おおやけ、コウ",
    L"広:ひろ-い、ひろ-がる、ひろ-げる、ひろ-まる、ひろ-める、コウ",
    L"交:まじ-わる、ま-ざる、ま-じる、ま-ぜる、まじ-える、コウ、か-う、か-わす",
    L"光:ひかり、ひか-る、コウ",
    L"考:かんが-える、コウ",
    L"行:い-く、おこな-う、ゆ-く、コウ、ギョウ",
    L"高:たか-い、たか、たか-まる、たか-める、コウ",
    L"黄:き、オウ、こ、コウ",
    L"合:あ-う、あわ-せる、ゴウ、ガッ、カッ、あわ-す",
    L"谷:たに、コク",
    L"国:くに、コク",
    L"黒:くろ、くろ-い、コク",
    L"今:いま、コン、キン",
    L"才:サイ",
    L"細:ほそ-い、こま-かい、サイ、ほそ-る、こま-か",
    L"作:つく-る、サク、サ",
    L"算:サン",
    L"止:と-める、と-まる、シ",
    L"市:いち、シ",
    L"矢:や、シ",
    L"姉:あね、シ",
    L"思:おもう、シ",
    L"紙:かみ、シ",
    L"寺:てら、ジ",
    L"自:みずか-ら、ジ、シ",
    L"時:とき、ジ",
    L"室:むろ、シツ",
    L"社:やしろ、シャ",
    L"弱:よわ-い、よわ-まる、よわ-める、ジャク、よわ-る",
    L"首:くび、シュ",
    L"秋:あき、シュウ",
    L"週:シュウ",
    L"春:はる、シュン",
    L"書:ショ、か-く",
    L"少:すく-ない、すこ-し、ショウ",
    L"場:ば",
    L"色:いろ",
    L"食:たべる",
    L"心:こころ",
    L"新:あたらしい",
    L"親:おや",
    L"図:ず",
    L"数:かず",
    L"西:にし",
    L"声:こえ",
    L"星:ほし",
    L"晴:はれ",
    L"切:きる",
    L"雪:ゆき",
    L"船:ふね",
    L"線:せん",
    L"前:まえ",
    L"組:くみ",
    L"走:はしる",
    L"多:おおい",
    L"太:ふとい",
    L"体:からだ",
    L"台:だい",
    L"地:ち",
    L"池:いけ",
    L"知:しる",
    L"茶:ちゃ",
    L"昼:ひる",
    L"長:ながい",
    L"鳥:とり",
    L"朝:あさ",
    L"直:じか",
    L"通:とおる",
    L"弟:おとうと",
    L"店:みせ",
    L"点:てん",
    L"電:でん",
    L"刀:かたな",
    L"冬:ふゆ",
    L"当:あたり",
    L"東:ひがし",
    L"答:こたえ",
    L"頭:あたま",
    L"同:おなじ",
    L"道:みち",
    L"読:よむ",
    L"内:うち",
    L"南:みなみ",
    L"肉:にく",
    L"馬:うま",
    L"売:うる",
    L"買:かう",
    L"麦:むぎ",
    L"半:はん",
    L"番:ばん",
    L"父:ちち",
    L"風:かぜ",
    L"分:ふん",
    L"聞:きく",
    L"米:こめ",
    L"歩:あるく",
    L"母:はは",
    L"方:ほう",
    L"北:きた",
    L"毎:まい",
    L"妹:いもうと",
    L"万:まん",
    L"明:あかるい",
    L"鳴:なる",
    L"毛:け",
    L"門:もん",
    L"夜:よる",
    L"野:の",
    L"友:とも",
    L"用:よう",
    L"曜:よう",
    L"来:くる",
    L"里:さと",
    L"理:り",
    L"話:はなし",
};

static const LPCWSTR g_aszMojiEnglish[] =
{
    L"引:",
    L"羽:",
    L"雲:",
    L"園:",
    L"遠:",
    L"何:",
    L"科:",
    L"夏:",
    L"家:",
    L"歌:",
    L"画:",
    L"回:",
    L"会:",
    L"海:",
    L"絵:",
    L"外:",
    L"角:",
    L"楽:",
    L"活:",
    L"間:",
    L"丸:",
    L"岩:",
    L"顔:",
    L"汽:",
    L"記:",
    L"帰:",
    L"弓:",
    L"牛:",
    L"魚:",
    L"京:",
    L"強:",
    L"教:",
    L"近:",
    L"兄:",
    L"形:",
    L"計:",
    L"元:",
    L"言:",
    L"原:",
    L"戸:",
    L"古:",
    L"午:",
    L"後:",
    L"語:",
    L"工:",
    L"公:",
    L"広:",
    L"交:",
    L"光:",
    L"考:",
    L"行:",
    L"高:",
    L"黄:",
    L"合:",
    L"谷:",
    L"国:",
    L"黒:",
    L"今:",
    L"才:",
    L"細:",
    L"作:",
    L"算:",
    L"止:",
    L"市:",
    L"矢:",
    L"姉:",
    L"思:",
    L"紙:",
    L"寺:",
    L"自:",
    L"時:",
    L"室:",
    L"社:",
    L"弱:",
    L"首:",
    L"秋:",
    L"週:",
    L"春:",
    L"書:",
    L"少:",
    L"場:",
    L"色:",
    L"食:",
    L"心:",
    L"新:",
    L"親:",
    L"図:",
    L"数:",
    L"西:",
    L"声:",
    L"星:",
    L"晴:",
    L"切:",
    L"雪:",
    L"船:",
    L"線:",
    L"前:",
    L"組:",
    L"走:",
    L"多:",
    L"太:",
    L"体:",
    L"台:",
    L"地:",
    L"池:",
    L"知:",
    L"茶:",
    L"昼:",
    L"長:",
    L"鳥:",
    L"朝:",
    L"直:",
    L"通:",
    L"弟:",
    L"店:",
    L"点:",
    L"電:",
    L"刀:",
    L"冬:",
    L"当:",
    L"東:",
    L"答:",
    L"頭:",
    L"同:",
    L"道:",
    L"読:",
    L"内:",
    L"南:",
    L"肉:",
    L"馬:",
    L"売:",
    L"買:",
    L"麦:",
    L"半:",
    L"番:",
    L"父:",
    L"風:",
    L"分:",
    L"聞:",
    L"米:",
    L"歩:",
    L"母:",
    L"方:",
    L"北:",
    L"毎:",
    L"妹:",
    L"万:",
    L"明:",
    L"鳴:",
    L"毛:",
    L"門:",
    L"夜:",
    L"野:",
    L"友:",
    L"用:",
    L"曜:",
    L"来:",
    L"里:",
    L"理:",
    L"話:",
};

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

    ZeroMemory(g_ahbmKanji2, sizeof(g_ahbmKanji2));
    for (j = 0; j < 160; ++j)
    {
        g_ahbmKanji2[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j));
        if (g_ahbmKanji2[j] == NULL)
            return FALSE;
    }

    g_hbm = NULL;

    try
    {
        InitKanji2();
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
            hbmOld = SelectObject(hdcMem, g_ahbmKanji2[g_map[j]]);
            rc.left = ix * (50 + 10) + 5 + 25;
            rc.top = iy * (50 + 10) + 5 + 10;
            if (iy >= 4)
                rc.top += 15;
            rc.right = rc.left + (50 + 10) - 10;
            rc.bottom = rc.top + (50 + 10) - 10;
            if (g_kanji2_history.find(j) != g_kanji2_history.end())
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

    v = g_kanji2_kakijun[g_nMoji];

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

    LPCWSTR psz = g_aszMojiReadings[g_nMoji];
    LPCWSTR pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption1, (pch + 1));

    psz = g_aszMojiEnglish[g_nMoji];
    pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption2, (pch + 1));

    g_kanji2_history.insert(nMoji);

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
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPWSTR psz = szText;
            LPWSTR pch = _tcschr(psz, TEXT(':'));
            *pch = 0;
            str = psz;
            wsprintf(szText, LoadStringDx(1000), str.c_str());
            ShellExecute(hwnd, NULL, szText, NULL, NULL, SW_SHOWNORMAL);
        }
        break;

    case 1001:
        {
            lstrcpyn(szText, g_aszMojiReadings[g_nMoji], _countof(szText));
            LPWSTR psz = szText;
            LPWSTR pch = _tcschr(psz, TEXT(':'));
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
    InitCommonControls();

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
