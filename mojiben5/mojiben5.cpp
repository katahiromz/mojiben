﻿// Moji No Benkyou (5)
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

#ifndef M_PI
    #define M_PI 3.141592653589
#endif

#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

#define SLIDE_TIMER 999

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
POINT g_ptDragging;

HBITMAP g_ahbmKanji2[160];
HBITMAP g_hbmClient = NULL;
HBITMAP g_hbmLeft = NULL;
HBITMAP g_hbmRight = NULL;

HBITMAP g_hbmKakijun;
INT g_nMoji;
HANDLE g_hThread;
HBRUSH g_hbrRed;
INT g_iPage = 0;
float g_eGoalPage = 0;
float g_eDisplayPage = 0;

std::set<INT> g_kanji2_history;

BOOL g_bHighSpeed = FALSE;

void DoSleep(DWORD dwMilliseconds)
{
    if (g_bHighSpeed)
        Sleep(dwMilliseconds / 10);
    else
        Sleep(dwMilliseconds);
}

static const LPCWSTR g_aszMojiReadings[] =
{
    L"引:ひ-く、イン",
    L"羽:はね、は、ウ",
    L"雲:くも、ウン",
    L"園:その、エン",
    L"遠:とお-い、エン",
    L"何:なに、なん、カ",
    L"科:カ",
    L"夏:なつ、カ、ゲ",
    L"家:いえ、や、カ、ケ",
    L"歌:うた、うた-う、カ",
    L"画:ガ、カク",
    L"回:まわ-る、まわ-す、カイ",
    L"会:あ-う、カイ、エ",
    L"海:うみ、カイ",
    L"絵:エ、カイ",
    L"外:そと、はず-す、はず-れる、ほか、ガイ、ゲ",
    L"角:かど、つの、カク",
    L"楽:ラク、ガク、たの-しい、たの-しむ",
    L"活:カツ、い-かす",
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
    L"計:はか-る、ケイ、はか-らう",
    L"元:もと、ゲン、ガン",
    L"言:いう、ゲン、こと、ゴン",
    L"原:はら、ゲン",
    L"戸:と、コ",
    L"古:ふる-い、コ",
    L"午:ゴ",
    L"後:うしろ、あと、のち、ゴ、コウ",
    L"語:かた-る、ゴ、かた-らう",
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
    L"書:か-く、ショ",
    L"少:すく-ない、すこ-し、ショウ",
    L"場:ば、ジョウ",
    L"色:いろ、ショク、シキ",
    L"食:た-べる、ショク、く-う、(ジキ)",
    L"心:こころ、シン",
    L"新:あたら-しい、シン、あら-た、にい",
    L"親:おや、した-しい、した-しむ、シン",
    L"図:ズ、ト、はか-る",
    L"数:かず、かぞ-える、スウ、ス",
    L"西:にし、セイ、サイ",
    L"声:こえ、セイ、こわ、(ショウ)",
    L"星:ほし、セイ、ショウ",
    L"晴:は-れる、は-らす、セイ",
    L"切:き-る、き-れる、セツ、サイ",
    L"雪:ゆき、セツ",
    L"船:ふね、セン、ふな",
    L"線:セン",
    L"前:まえ、ゼン",
    L"組:くみ、く-む、ソ",
    L"走:はし-る、ソウ",
    L"多:おお-い、タ",
    L"太:ふと-い、ふと-る、タ、タイ",
    L"体:からだ、タイ、テイ",
    L"台:ダイ、タイ",
    L"地:チ、ジ",
    L"池:いけ、チ",
    L"知:し-る、チ",
    L"茶:チャ、サ",
    L"昼:ひる、チュウ",
    L"長:なが-い、チョウ、(おさ)",
    L"鳥:とり、チョウ",
    L"朝:あさ、チョウ",
    L"直:なお-す、なお-る、チョウ、ジキ、ただ-ちに、(じか)",
    L"通:とお-る、とお-す、かよ-う、ツウ、(ツ)",
    L"弟:おとうと、ダイ、テ",
    L"店:みせ、テン",
    L"点:テン、(つ-ける)",
    L"電:デン",
    L"刀:かたな、トウ",
    L"冬:ふゆ、トウ",
    L"当:あ-たる、あ-てる、トウ",
    L"東:ひがし、トウ、(あずま)",
    L"答:こた-え、こた-える、トウ",
    L"頭:あたま、トウ、ズ、かしら、(ト)",
    L"同:おな-じ、ドウ",
    L"道:みち、ドウ",
    L"読:よ-む、ドク、トク、トウ",
    L"内:うち、ナイ、ダイ",
    L"南:みなみ、ナン、(ナ)",
    L"肉:ニク",
    L"馬:うま、バ、(ま)",
    L"売:う-る、う-れる、バイ",
    L"買:か-う、バイ",
    L"麦:むぎ、バク",
    L"半:はん、なか-ば",
    L"番:バン、(つが-い)",
    L"父:ちち、フ",
    L"風:かぜ、かざ、フウ、(フ)",
    L"分:わ-ける、わ-かる、わ-かれる、フン、ブン、プン、ブ、わ-かつ",
    L"聞:き-く、き-こえる、ブン、モン",
    L"米:こめ、ベイ、マイ、メートル",
    L"歩:ある-く、ホ、ブ、あゆ-む、(フ)",
    L"母:はは、ボ",
    L"方:ホウ、かた",
    L"北:きた、ホク、ボク",
    L"毎:マイ、(ごと)",
    L"妹:いもうと、マイ",
    L"万:マン、バン、(よろず)",
    L"明:あか-るい、あ-かす、あ-ける、あき-らか、メイ、ミョウ",
    L"鳴:な-く、な-らす、な-る、メイ",
    L"毛:け、モウ",
    L"門:モン、かど",
    L"夜:よる、よ、ヤ",
    L"野:の、ヤ",
    L"友:とも、ユウ",
    L"用:もち-いる、ヨウ",
    L"曜:ヨウ",
    L"来:く-る、ライ、き-たる、き-たす",
    L"里:さと、リ",
    L"理:リ",
    L"話:はなし、はな-す、ワ",
};

static const LPCWSTR g_aszMojiEnglish[] =
{
    L"引:Pull",
    L"羽:Feather, Wing, Plume",
    L"雲:Cloud",
    L"園:Garden, Park, Plantation",
    L"遠:Far, Distant",
    L"何:What",
    L"科:Department, Section, (Science)",
    L"夏:Summer",
    L"家:House, Home, Residence",
    L"歌:Song, Sing",
    L"画:Picture, Stroke",
    L"回:Rotate, Rolling, Times",
    L"会:Meating, Association, Party, Club, Assembly",
    L"海:Sea",
    L"絵:Picture, Painting, Drawing, Illustration, Sketch",
    L"外:Outside, Foreign",
    L"角:Corner, Angle, Horn",
    L"楽:Easy, Comfort, Funny",
    L"活:Living, Life, Active",
    L"間:Interval, Space, Pause, Room",
    L"丸:Circle, Pill, (Ship)",
    L"岩:Rock",
    L"顔:Face",
    L"汽:Steam",
    L"記:Note, Chronicle",
    L"帰:Return",
    L"弓:Bow",
    L"牛:Cattle, Cow",
    L"魚:Fish",
    L"京:Capital, (Kyoto), 10,000,000,000,000,000",
    L"強:Strong, Intense",
    L"教:Teaching",
    L"近:Near",
    L"兄:Older Brother",
    L"形:Shape, Form, Type",
    L"計:Total, Calculate",
    L"元:Former, Origin, Yuan",
    L"言:Say, Words",
    L"原:Original, Primitive, Field",
    L"戸:Door",
    L"古:Old, Used, Ancient",
    L"午:Noon",
    L"後:Rear, Later, Remainder",
    L"語:Word, Language",
    L"工:Engineering",
    L"公:Public, Open, Lord",
    L"広:Wide, Widen, Spread",
    L"交:Exchange, Crossing",
    L"光:Light, Shine",
    L"考:Thoughts, Think",
    L"行:Line, Go, Row",
    L"高:High, Expensive",
    L"黄:Yellow",
    L"合:Match, Fit",
    L"谷:Valley",
    L"国:Country",
    L"黒:Black, Dark",
    L"今:Now, This, Immediately",
    L"才:Talent, Years Old",
    L"細:Thin, Detail",
    L"作:Make, Work, Product",
    L"算:Calculate, Arithmetic",
    L"止:Stop",
    L"市:City, Market, Town, Fair",
    L"矢:Arrow",
    L"姉:Older Sister",
    L"思:Thoughts, Fealing",
    L"紙:Paper",
    L"寺:Temple",
    L"自:Self",
    L"時:Time, Hour, Occasion",
    L"室:Room",
    L"社:Company, Shinto Shrine",
    L"弱:Weak, Feeble",
    L"首:Neck",
    L"秋:Autumn, Fall",
    L"週:Week",
    L"春:Springtime",
    L"書:Write, Calligraphy",
    L"少:Few, Scarce, Little",
    L"場:Place, Field",
    L"色:Color, Variety, (Sexy)",
    L"食:Food, Eat, Eclipse",
    L"心:Heart, Mind, Core",
    L"新:New",
    L"親:Parent, Familiar",
    L"図:Figure, Drawing, Attempt",
    L"数:Number",
    L"西:West",
    L"声:Voice, Opinion",
    L"星:Star",
    L"晴:Sunny",
    L"切:Cut, Eager, Acute, Keen",
    L"雪:Snow",
    L"船:Boat, Ship, Vessel",
    L"線:Line, Wire, Beam",
    L"前:Before, Previous, Former, Front",
    L"組:Group, Set, Class",
    L"走:Run",
    L"多:Many",
    L"太:Thick",
    L"体:Body, Form, Substance, Health",
    L"台:Stand, Table, Pedestal, Support",
    L"地:Earth, Ground",
    L"池:Pond",
    L"知:Know, Knowledge",
    L"茶:Tea, Brown",
    L"昼:Noon, Daytime",
    L"長:Long, Chief, Boss",
    L"鳥:Bird, Poultry, Fowl",
    L"朝:Morning, Imperial",
    L"直:Straight, Direct, Fix",
    L"通:Go, Street, Expert, Understand",
    L"弟:Younger Brother",
    L"店:Shop, Store",
    L"点:Point, Dot, Spot, Lighting",
    L"電:Electric",
    L"刀:Katana, Knife, Blade, Japanese Sword",
    L"冬:Winter",
    L"当:Current, Hit",
    L"東:East",
    L"答:Answer",
    L"頭:Head, (counter for large animals)",
    L"同:Same",
    L"道:Road, Way, Method",
    L"読:Read",
    L"内:Inside",
    L"南:South",
    L"肉:Meat, Flesh",
    L"馬:Horse",
    L"売:Sell",
    L"買:Buy",
    L"麦:Wheat, Barley",
    L"半:Half",
    L"番:#, Watching",
    L"父:Father",
    L"風:Wind, Breeze, Blast, Style",
    L"分:Minutes, Divide, Branch, Rate",
    L"聞:Listen, Hear",
    L"米:Rice, (Meter), (America)",
    L"歩:Walk, Step, Stride, Pawn",
    L"母:Mother",
    L"方:Direction, Person, Side",
    L"北:North",
    L"毎:Every, Each",
    L"妹:Younger Sister",
    L"万:Ten Thousand, All",
    L"明:Bright, Luminous, Enlightened",
    L"鳴:Sound, Ring, Echo",
    L"毛:Hair, Fur",
    L"門:Gate",
    L"夜:Night",
    L"野:Field",
    L"友:Friend, Companion, Pal",
    L"用:Use, Business",
    L"曜:Day Of The Week",
    L"来:Next, Come",
    L"里:Village, Home Country, (Mile)",
    L"理:Reason, (Science)",
    L"話:Story, Talk, Speech, Chat",
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
    g_hThread = NULL;
    g_hbmKakijun = NULL;
    g_hbrRed = CreateSolidBrush(RGB(255, 0, 0));

    g_hbmLeft = LoadBitmap(g_hInstance, MAKEINTRESOURCE(100));
    g_hbmRight = LoadBitmap(g_hInstance, MAKEINTRESOURCE(101));

    HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, 0x3340, LoadStringDx(5));
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, 0x3330, LoadStringDx(2));

    ZeroMemory(g_ahbmKanji2, sizeof(g_ahbmKanji2));
    for (INT j = 0; j < 160; ++j)
    {
        g_ahbmKanji2[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j));
        if (g_ahbmKanji2[j] == NULL)
            return FALSE;
    }

    g_hbmClient = NULL;

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

#define COLUMNS 10
#define ROWS 7

VOID GetMojiRect(INT j, LPRECT prc)
{
    INT iPage = j / (ROWS * COLUMNS);
    j %= (ROWS * COLUMNS);
    INT ix = j % COLUMNS;
    INT iy = j / COLUMNS;
    RECT rc;
    rc.left = ix * (50 + 10) + 5 + 25;
    rc.top = iy * (50 + 10) + 5 + 10;
    rc.right = rc.left + (50 + 10) - 10;
    rc.bottom = rc.top + (50 + 10) - 10;
    OffsetRect(&rc, iPage * (50 + 10) * COLUMNS, 0);
    OffsetRect(&rc, (LONG)(-g_eDisplayPage * (50 + 10) * COLUMNS), 0);
    *prc = rc;
}

INT GetNumPage(VOID)
{
    return (_countof(g_ahbmKanji2) + COLUMNS * ROWS - 1) / (COLUMNS * ROWS);
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

        for (j = 0; j < _countof(g_ahbmKanji2); ++j)
        {
            GetMojiRect(j, &rc);
            hbmOld = SelectObject(hdcMem, g_ahbmKanji2[g_map[j]]);
            if (g_kanji2_history.find(j) != g_kanji2_history.end())
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

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    if (HDC hdc = BeginPaint(hwnd, &ps))
    {
        OnDraw(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
}

static unsigned ThreadProcWorker(void)
{
    RECT rc;
    SIZE siz;
    CBitmap hbm1, hbm2;
    HGDIOBJ hbmOld;
    INT k;
    POINT apt[4];
    double cost, sint, cost2, sint2;

    GetClientRect(g_hKakijunWnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    const std::vector<GA>& v = g_kanji2_kakijun[g_nMoji];

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
        SelectObject(hdcMem, hbmOld);
    }

    if (g_hbmKakijun)
        ::DeleteObject(g_hbmKakijun);
    g_hbmKakijun = hbm1;
    InvalidateRect(g_hKakijunWnd, NULL, FALSE);

    ShowWindow(g_hKakijunWnd, SW_SHOWNORMAL);
    DoSleep(300);

    PlaySound(MAKEINTRESOURCE(1000 + g_nMoji), g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
    PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

    CRgn hRgn5(::CreateRectRgn(0, 0, 0, 0));
    for (UINT i = 0; i < v.size(); ++i)
    {
        switch (v[i].type)
        {
        case WAIT:
            DoSleep(500);

            if (!IsWindowVisible(g_hKakijunWnd))
                return 0;

            // ポンと音を鳴らす。
            PlaySound(MAKEINTRESOURCE(100), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
            break;

        case LINEAR:
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
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                CRgn hRgn2(::ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb));

                cost = std::cos(v[i].angle0 * M_PI / 180);
                sint = std::sin(v[i].angle0 * M_PI / 180);

                // NULLREGIONでない場所を探す。
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

                // NULLREGIONでない位置から赤い画を描画する。
                for ( ; k < 200; k += 20)
                {
                    if (!IsWindowVisible(g_hKakijunWnd))
                        return 0;

                    hbm1.Swap(hbm2);

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

                    g_hbmKakijun = hbm1;
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

                hbmOld = SelectObject(hdcMem, hbm1);
                rc.left = 0;
                rc.top = 0;
                rc.right = siz.cx;
                rc.bottom = siz.cy;
                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
                SelectObject(hdcMem, hbmOld);

                g_hbmKakijun = hbm1;

                CRgn hRgn2(::ExtCreateRegion(NULL, v[i].cb, (RGNDATA *)v[i].pb));
                if (v[i].angle0 <= v[i].angle1)
                {
                    for (k = v[i].angle0; k < v[i].angle1; k += 20)
                    {
                        if (!IsWindowVisible(g_hKakijunWnd))
                            return 0;
                        double theta = k * M_PI / 180.0;
                        double theta2 = (k + 20) * M_PI / 180.0;
                        cost = std::cos(theta);
                        sint = std::sin(theta);
                        cost2 = std::cos(theta2);
                        sint2 = std::sin(theta2);
                        hbm1.Swap(hbm2);

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

                        g_hbmKakijun = hbm1;

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
                        cost = std::cos(theta);
                        sint = std::sin(theta);
                        cost2 = std::cos(theta2);
                        sint2 = std::sin(theta2);
                        hbm1.Swap(hbm2);

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

                        g_hbmKakijun = hbm1;

                        InvalidateRect(g_hKakijunWnd, NULL, FALSE);
                        if (n == NULLREGION)
                            break;
                        DoSleep(35);
                    }
                }
            }
            break;
        }
    }

    DoSleep(500);

    {
        CDC hdc(g_hKakijunWnd);
        CDC hdcMem(hdc);
        hbm1.Swap(hbm2);

        hbmOld = SelectObject(hdcMem, hbm1);
        rc.left = 0;
        rc.top = 0;
        rc.right = siz.cx;
        rc.bottom = siz.cy;
        FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        FillRgn(hdcMem, hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SelectObject(hdcMem, hbmOld);

        g_hbmKakijun = hbm1;
        InvalidateRect(g_hKakijunWnd, NULL, FALSE);
    }

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

    LPCWSTR psz = g_aszMojiReadings[g_nMoji];
    LPCWSTR pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption1, (pch + 1));

    psz = g_aszMojiEnglish[g_nMoji];
    pch = _tcschr(psz, TEXT(':'));
    SetWindowText(g_hwndCaption2, (pch + 1));

    g_kanji2_history.insert(nMoji);

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

    if (GetLeftArrowRect(hwnd, &rc))
    {
        if (PtInRect(&rc, pt))
        {
            PlaySound(MAKEINTRESOURCE(101), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
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
            PlaySound(MAKEINTRESOURCE(101), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
            g_eDisplayPage = (float)g_iPage;
            g_eGoalPage = (float)(g_iPage + 1);
            SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
            return;
        }
    }

    for (j = 0; j < _countof(g_ahbmKanji2); ++j)
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

    for (UINT j = 0; j < _countof(g_ahbmKanji2); ++j)
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

    return TRUE;
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
    if (g_hThread)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
        CloseHandle(g_hThread);
    }

    DeleteObject(g_hFont);
    DeleteObject(g_hFontSmall);

    UINT i;
    for (i = 0; i < _countof(g_ahbmKanji2); ++i)
    {
        if (g_ahbmKanji2[i])
            DeleteObject(g_ahbmKanji2[i]);
    }

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmLeft);
    DeleteObject(g_hbmRight);

    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbrRed);

    g_kanji2_history.clear();

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
    if ((cmd & 0xFFF0) == 0x3340)
    {
        g_bHighSpeed = !g_bHighSpeed;
        HMENU hSysMenu = ::GetSystemMenu(hwnd, FALSE);
        ::CheckMenuItem(hSysMenu, 0x3340, (g_bHighSpeed ? MF_CHECKED : MF_UNCHECKED));
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

void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (!fDown)
        return;

    if (vk == VK_ESCAPE)
    {
        ShowWindow(g_hKakijunWnd, SW_HIDE);
    }
}

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
        PlaySound(MAKEINTRESOURCE(101), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        g_eDisplayPage = (float)g_iPage;
        g_eGoalPage = (float)(g_iPage - 1);
        SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
        return;
    }

    if (eDelta == +1 && g_iPage + 1 < GetNumPage())
    {
        PlaySound(MAKEINTRESOURCE(101), g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
        g_eDisplayPage = (float)g_iPage;
        g_eGoalPage = (float)(g_iPage + 1);
        SetTimer(hwnd, SLIDE_TIMER, 50, NULL);
        return;
    }
}

void OnCancelMode(HWND hwnd)
{
    ReleaseCapture();
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    ReleaseCapture();
}

void OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
    ReleaseCapture();
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

    // Detect handle leaks
    OBJECTS_CHECK_POINT();

    // Detect memory leaks
#if defined(_MSC_VER) && !defined(NDEBUG) // Visual C++ only
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return (INT)msg.wParam;
}
