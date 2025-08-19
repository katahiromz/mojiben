// Moji No Benkyou (7)
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

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (7)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (7) Kakijun");
static const TCHAR g_szCaptionClassName[] = TEXT("Moji No Benkyou (7) Caption");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;
POINT g_ptDragging;

HBITMAP g_ahbmKanji4[202];
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

std::set<INT> g_kanji3_history;

BOOL g_bHighSpeed = FALSE;

void DoSleep(DWORD dwMilliseconds)
{
    if (g_bHighSpeed)
        Sleep(dwMilliseconds / 10);
    else
        Sleep(dwMilliseconds);
}

static const LPCWSTR g_aszMojiReadings[202] =
{
    L"愛:あい-する、アイ",
    L"案:アン",
    L"以:イ、(もっ-て)",
    L"衣:ころも、イ",
    L"位:くらい、イ",
    L"茨:いばら",
    L"印:しるし、イン",
    L"英:エイ",
    L"栄:さか-える、エイ",
    L"媛:エン、(ひめ)",
    L"塩:しお、エン",
    L"岡:おか",
    L"億:オク",
    L"加:くわ-える、くわ-わる、カ",
    L"果:は-たす、は-てる、カ",
    L"貨:カ",
    L"課:カ",
    L"芽:め、ガ",
    L"賀:ガ",
    L"改:あらた-める、あらた-まる、カイ",
    L"械:カイ",
    L"害:ガイ",
    L"街:まち、ガイ",
    L"各:カク、(おのおの)",
    L"覚:おぼ-える、さ-める、さ-ます、カク",
    L"潟:かた",
    L"完:カン",
    L"官:カン",
    L"管:くだ、カン",
    L"関:かか-わる、せき、カン",
    L"観:カン、(み-る)",
    L"願:ねが-う、ガン",
    L"岐:キ、ギ",
    L"希:キ",
    L"季:キ",
    L"旗:はた、キ",
    L"器:うつわ、キ",
    L"機:キ、(はた)",
    L"議:ギ",
    L"求:もと-める、キュウ",
    L"泣:な-く、キュウ",
    L"給:キュウ、(たま-う)、(たま-わる)",
    L"挙:あ-げる、あ-がる、キョ",
    L"漁:ギョ、リョウ",
    L"共:とも、キョウ",
    L"協:キョウ",
    L"鏡:かがみ、キョウ",
    L"競:きそ-う、キョウ、ケイ、(せ-る)",
    L"極:きわ-める、きわ-まる、キョク、ゴク",
    L"熊:くま",
    L"訓:クン",
    L"軍:グン",
    L"郡:グン、(こおり)",
    L"群:む-れ、グン、む-れる",
    L"径:ケイ",
    L"景:ケイ、ケ",
    L"芸:ゲイ",
    L"欠:か-ける、ケツ、か-く",
    L"結:むす-ぶ、ケツ、ゆ-う",
    L"建:た-てる、た-つ、ケン",
    L"健:すこ-やか、ケン",
    L"験:ケン",
    L"固:かた-める、かた-まる、かた-い、コ",
    L"功:コウ",
    L"好:この-む、す-き、コウ",
    L"香:かお-る、か、コウ",
    L"候:コウ、(そうろう)",
    L"康:コウ",
    L"佐:サ",
    L"差:さ-す、サ",
    L"菜:な、サイ",
    L"最:もっと-も、サイ",
    L"埼:さい",
    L"材:ザイ",
    L"崎:さき",
    L"昨:サク",
    L"札:ふだ、サツ",
    L"刷:す-る、サツ",
    L"察:サツ",
    L"参:まい-る、サン",
    L"産:う-む、う-まれる、サン",
    L"散:ち-る、ち-らす、ち-らかす、ち-らかる、サン",
    L"残:のこ-る、のこ-す、ザン",
    L"氏:シ、うじ",
    L"司:シ、(つかさど-る)",
    L"試:ため-す、こころ-みる、シ",
    L"児:ジ、ニ",
    L"治:なお-す、なお-る、おさ-める、ジ、チ",
    L"滋:ジ",
    L"辞:ジ、や-める",
    L"鹿:しか、か",
    L"失:うしな-う、シツ、(う-せる)",
    L"借:か-りる、シャク",
    L"種:たね、シュ",
    L"周:まわ-り、シュウ",
    L"祝:いわ-う、シュク",
    L"順:ジュン",
    L"初:はじ-めて、はじ-め、はつ、ショ",
    L"松:まつ、ショウ",
    L"笑:わら-う、ショウ、(え-む)",
    L"唱:とな-える、ショウ",
    L"焼:や-く、や-ける、ショウ",
    L"照:て-らす、て-る、て-れる、ショウ",
    L"城:しろ、ジョウ",
    L"縄:なわ、ジョウ",
    L"臣:ジン、(おみ)",
    L"信:シン-じる、シン",
    L"井:イ、ショウ",
    L"成:な-る、な-す、セイ",
    L"省:はぶ-く、かえり-みる、セイ、ショウ",
    L"清:きよ-い、きよ-める、セイ、(ショウ)",
    L"静:しず-か、しず-める、しず-まる、セイ",
    L"席:セキ",
    L"積:つ-もる、つ-む、セキ",
    L"折:お-る、お-れる、お-り、セツ",
    L"節:ふし、セツ",
    L"説:セツ、と-く",
    L"浅:あさ-い、セン",
    L"戦:たたか-う、いくさ、セン",
    L"選:えら-ぶ、セン",
    L"然:ゼン、ネン",
    L"争:あらそ-う、ソウ",
    L"倉:くら、ソウ",
    L"巣:す、ソウ",
    L"束:たば、ソク",
    L"側:がわ、ソク",
    L"続:つづ-く、つづ-ける、ゾク",
    L"卒:ソツ",
    L"孫:まご、ソン",
    L"帯:おび、お-びる、タイ",
    L"隊:タイ",
    L"達:たっ-する、タツ、(たち)",
    L"単:タン、(ひとえ)",
    L"置:お-く、チ",
    L"仲:なか、チュウ",
    L"沖:おき、(チュウ)",
    L"兆:チョウ、(きざ-し)",
    L"低:ひく-い、テイ",
    L"底:そこ、テイ",
    L"的:まと、テキ",
    L"典:テン、(のり)",
    L"伝:つた-える、つた-わる、つた-う、デン",
    L"徒:ト",
    L"努:つと-める、ド",
    L"灯:トウ、(ひ)",
    L"働:はたら-く、ドウ",
    L"特:トク",
    L"徳:トク",
    L"栃:とち",
    L"奈:ナ",
    L"梨:なし",
    L"熱:ネツ、あつ-い",
    L"念:ネン",
    L"敗:やぶ-れる、ハイ",
    L"梅:うめ、バイ",
    L"博:ハク、(バク)",
    L"阪:ハン、(さか)",
    L"飯:めし、ハン",
    L"飛:と-ぶ、と-ばす、ヒ",
    L"必:かなら-ず、ヒツ",
    L"票:ヒョウ",
    L"標:ヒョウ",
    L"不:フ、ブ",
    L"夫:おっと、フ",
    L"付:つ-ける、つ-く、フ",
    L"府:フ",
    L"阜:フ",
    L"富:と-む、とみ、フ、(フウ)",
    L"副:フク",
    L"兵:ヘイ、(つわもの)",
    L"別:わか-れる、ベツ",
    L"辺:あた-り、べ、ヘン",
    L"変:か-える、か-わる、ヘン",
    L"便:たよ-り、ベン、ビン",
    L"包:つつ-む、ホウ",
    L"法:ホウ",
    L"望:のぞ-む、ボウ、(モウ)",
    L"牧:まき、ボク",
    L"末:すえ、マツ",
    L"満:み-たす、み-ちる、マン",
    L"未:ミ、(ひつじ)",
    L"民:たみ、ミン",
    L"無:な-い、ム、(ブ)",
    L"約:ヤク",
    L"勇:ユウ、いさ-ましい",
    L"要:かなめ、ヨウ、(い-る)",
    L"養:やしな-う、ヨウ",
    L"浴:あ-びる、あ-びせる、ヨク",
    L"利:リ、(き-く)",
    L"陸:リク",
    L"良:よ-い、リョウ",
    L"料:リョウ",
    L"量:リョウ、はか-る",
    L"輪:リン、わ",
    L"類:たぐ-い、ルイ",
    L"令:レイ",
    L"冷:つめ-たい、さ-める、さ-ます、ひ-やかす、レイ",
    L"例:たと-える、レイ",
    L"連:つ-れる、つら-なる、つら-ねる、レン",
    L"老:お-いる、ロウ",
    L"労:ロウ、(いた-わる)",
    L"録:ロク",
};

static const LPCWSTR g_aszMojiEnglish[202] =
{
    L"愛:Loving",
    L"案:Proposal, Plan, Draft",
    L"以:(By using ...)",
    L"衣:Clothing, Dressing",
    L"位:Rank, Grade, Throne",
    L"茨:Thorns",
    L"印:Mark, Symbol, Stamp, (India)",
    L"英:English, Excellent, Outstanding",
    L"栄:Thrive",
    L"媛:(Princess)",
    L"塩:Salt",
    L"岡:Hill",
    L"億:Hundred Million, 100000000",
    L"加:Addition, (Canada)",
    L"果:Fruit, Fulfill",
    L"貨:Currency, Cargo, Freight",
    L"課:Division, Impose",
    L"芽:Bud, Sprout",
    L"賀:-",
    L"改:Revised, Renew",
    L"械:(Machine)",
    L"害:Harm",
    L"街:Street, Town",
    L"各:Each",
    L"覚:Awakening",
    L"潟:Lagoon",
    L"完:Complete",
    L"官:Government",
    L"管:Tube, Pipe, (Managing)",
    L"関:Barrier, Gate, Concern",
    L"観:View, Sightseeing, Observe",
    L"願:Wish",
    L"岐:-",
    L"希:Rare, Hope",
    L"季:(Season)",
    L"旗:Flag",
    L"器:Vessel, Bowl, Container",
    L"機:(Machine), (Loom)",
    L"議:(Discussion)",
    L"求:Request",
    L"泣:Cry",
    L"給:Supply, Salary, Wage",
    L"挙:(Give an example), (Raise), (Behavior)",
    L"漁:Fishing",
    L"共:(Jointly)",
    L"協:Cooperate",
    L"鏡:Mirror",
    L"競:Competition",
    L"極:Very, Quite",
    L"熊:Bear (animal)",
    L"訓:Japanese reading (of kanji), Instruction",
    L"軍:Army, Troops",
    L"郡:County, District",
    L"群:Group, Flock",
    L"径:Diameter",
    L"景:Scenery",
    L"芸:Art",
    L"欠:Missing",
    L"結:Conclusion, Tying, Binding",
    L"建:Building, Construction",
    L"健:(Healthy)",
    L"験:Experience, Try, Test",
    L"固:Hard, Solid, (Fixed)",
    L"功:Merit, Achievement",
    L"好:Liking, Favorite",
    L"香:Scent, Incense",
    L"候:Weather, Season, Time, (Candidate)",
    L"康:Health",
    L"佐:-",
    L"差:Difference, Variation, (Insert), (Use an umbrella)",
    L"菜:Vegetables, Greens",
    L"最:Most",
    L"埼:-",
    L"材:Material",
    L"崎:-",
    L"昨:Last",
    L"札:Paper Money, Token",
    L"刷:Printing",
    L"察:(Inspection), (Police)",
    L"参:Visit (a shrine), Come, Defeated",
    L"産:Production, Give birth",
    L"散:Scattered, (Take a walk)",
    L"残:Remaining",
    L"氏:Mr., Family name",
    L"司:Govern",
    L"試:Trial, Test, Try",
    L"児:(Child), (Kid)",
    L"治:Healing, Govern",
    L"滋:(Nourish)",
    L"辞:Words, Resign, Decline",
    L"鹿:Deer",
    L"失:Loss, Failure",
    L"借:Debt, Borrowing",
    L"種:Seed, Species, Trick",
    L"周:Surroundings",
    L"祝:Congratulations",
    L"順:Order",
    L"初:First time",
    L"松:Pine tree",
    L"笑:Laughing, Smile",
    L"唱:Chanting",
    L"焼:Grill, Burn, Bake",
    L"照:Illuminate, Shine",
    L"城:Castle",
    L"縄:Rope",
    L"臣:(Minister)",
    L"信:Trust, Faith",
    L"井:Well (of water)",
    L"成:Growth, Become",
    L"省:Omit, Ministry",
    L"清:Clean, Pure",
    L"静:Silence, Quiet",
    L"席:Seat",
    L"積:Pile up, Product (of numbers)",
    L"折:Fold",
    L"節:Section, Node, Joint",
    L"説:Theory, Explain",
    L"浅:Shallow",
    L"戦:War, Battle, Fight",
    L"選:Selection, Election, Choose",
    L"然:Naturally",
    L"争:Compete, Dispute",
    L"倉:Storehouse, Warehouse",
    L"巣:Nest, Beehive",
    L"束:Bundle, Bunch",
    L"側:Side, -side",
    L"続:Continue",
    L"卒:Graduate, Die, Finish",
    L"孫:Grandchild",
    L"帯:Belt, Band, Obi",
    L"隊:Squad",
    L"達:Reach, Arrive, (Multiple people)",
    L"単:(Single)",
    L"置:Put, Place",
    L"仲:Relationship",
    L"沖:Oki",
    L"兆:Trillion, (Signs)",
    L"低:Low",
    L"底:Bottom",
    L"的:Target, (-like)",
    L"典:(Ceremony), (Law), (Typical)",
    L"伝:Tell, Transmission, Biography, Legend",
    L"徒:(Student), (Party)",
    L"努:Strive, Exert",
    L"灯:Light (of fire)",
    L"働:Work, Labor",
    L"特:Special",
    L"徳:Virtue",
    L"栃:-",
    L"奈:-",
    L"梨:Pear",
    L"熱:Heat, Fever",
    L"念:Thought",
    L"敗:Defeat",
    L"梅:Plum",
    L"博:Expo, Museum, (Doctorate), (Gamble)",
    L"阪:(Osaka)",
    L"飯:Rice, Meals",
    L"飛:Fly",
    L"必:Must",
    L"票:Vote",
    L"標:Mark, Sign, Elevation, Standard",
    L"不:Not doing",
    L"夫:Husband",
    L"付:Attach, Wear, With",
    L"府:(Prefecture)",
    L"阜:-",
    L"富:Wealth",
    L"副:Deputy, Assistant, Auxiliaty",
    L"兵:Soldier, Warrior",
    L"別:Another, Different, Separate",
    L"辺:Area, Edge (of math)",
    L"変:Change, Strange, Funny",
    L"便:Convenience, Flight, Mail, Excrement",
    L"包:Wrap, Pack, (Package)",
    L"法:Law",
    L"望:Desire, Hope",
    L"牧:(Ranch), (Pasture)",
    L"末:End, Last",
    L"満:Full, Satisfy, Fulfill",
    L"未:Not yet",
    L"民:Nation, Private",
    L"無:Nothing",
    L"約:Approximately, Promise",
    L"勇:Courage, Bravery",
    L"要:Need, Main point",
    L"養:Raise, Nourish",
    L"浴:Bath, Bathe",
    L"利:Advantage, Interest, Profit, Benefit",
    L"陸:Land",
    L"良:Good, Excellent",
    L"料:Fee, (Cooking)",
    L"量:Amount, Quantity, Volume",
    L"輪:Ring, Loop, Hoop, (Wheel)",
    L"類:Kind (of something)",
    L"令:(Command), (Order)",
    L"冷:Cold, Cool",
    L"例:Example",
    L"連:Consecutive, (Connect), (Link)",
    L"老:Old person",
    L"労:Labor",
    L"録:Record",
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

    ZeroMemory(g_ahbmKanji4, sizeof(g_ahbmKanji4));
    for (UINT j = 0; j < _countof(g_ahbmKanji4); ++j)
    {
        g_ahbmKanji4[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j));
        if (g_ahbmKanji4[j] == NULL)
            return FALSE;
    }

    g_hbmClient = NULL;

    try
    {
        InitKanji4();
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
    return (_countof(g_ahbmKanji4) + COLUMNS * ROWS - 1) / (COLUMNS * ROWS);
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

        for (j = 0; j < _countof(g_ahbmKanji4); ++j)
        {
            GetMojiRect(j, &rc);
            hbmOld = SelectObject(hdcMem, g_ahbmKanji4[g_map[j]]);
            if (g_kanji3_history.find(j) != g_kanji3_history.end())
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

    const std::vector<GA>& v = g_kanji4_kakijun[g_nMoji];

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

    g_kanji3_history.insert(nMoji);

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

    for (j = 0; j < _countof(g_ahbmKanji4); ++j)
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

    for (UINT j = 0; j < _countof(g_ahbmKanji4); ++j)
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
        HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
        SIZE size;
        GetTextExtentPoint32W(hdc, szText, lstrlen(szText), &size);
        SelectObject(hdc, hFontOld);
        if (size.cx >= 550)
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
        HGDIOBJ hFontOld = SelectObject(hdc, g_hFont);
        SIZE size;
        GetTextExtentPoint32W(hdc, szText, lstrlen(szText), &size);
        SelectObject(hdc, hFontOld);
        if (size.cx >= 550)
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
    for (i = 0; i < _countof(g_ahbmKanji4); ++i)
    {
        if (g_ahbmKanji4[i])
            DeleteObject(g_ahbmKanji4[i]);
    }

    DeleteObject(g_hbmClient);
    DeleteObject(g_hbmLeft);
    DeleteObject(g_hbmRight);

    DeleteObject(g_hbmKakijun);
    DeleteObject(g_hbrRed);

    g_kanji3_history.clear();

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
