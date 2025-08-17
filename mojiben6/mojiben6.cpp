// Moji No Benkyou (6)
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

static const TCHAR g_szClassName[] = TEXT("Moji No Benkyou (6)");
static const TCHAR g_szKakijunClassName[] = TEXT("Moji No Benkyou (6) Kakijun");
static const TCHAR g_szCaptionClassName[] = TEXT("Moji No Benkyou (6) Caption");

HINSTANCE g_hInstance;
HWND g_hMainWnd;
HWND g_hKakijunWnd;
HWND g_hwndCaption1 = NULL;
HWND g_hwndCaption2 = NULL;
HFONT g_hFont;
HFONT g_hFontSmall;
POINT g_ptDragging;

HBITMAP g_ahbmKanji3[200];
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

static const LPCWSTR g_aszMojiReadings[200] =
{
    L"悪:わる-い、アク",
    L"安:やす-い、やす-らか、アン",
    L"暗:くら-い、アン",
    L"医:イ",
    L"委:ゆだ-ねる、イ",
    L"意:イ",
    L"育:そだ-つ、そだ-てる、はぐく-む、イク",
    L"員:イン",
    L"院:イン",
    L"飲:の-む、イン",
    L"運:はこ-ぶ、ウン",
    L"泳:およ-ぐ、エイ",
    L"駅:エキ",
    L"央:オウ",
    L"横:よこ、オウ",
    L"屋:や、オク",
    L"温:あたた-かい、あたた-まる、あたた-める、オン、(ウン)",
    L"化:ば-ける、ば-かす、カ、ケ",
    L"荷:に、カ",
    L"界:カイ",
    L"開:あ-ける、ひら-く、あ-く、ひら-ける、カイ",
    L"階:カイ",
    L"寒:さむ-い、カン",
    L"感:かん-じる、カン",
    L"漢:カン、(おとこ)",
    L"館:やかた、カン、(たち)",
    L"岸:きし、ガン",
    L"起:お-きる、お-こす、お-こる、キ",
    L"期:キ",
    L"客:キャク、カク",
    L"究:きわ-める、キュウ",
    L"急:いそ-ぐ、キュウ",
    L"級:キュウ",
    L"宮:みや、キュウ、グウ",
    L"球:キュウ、たま",
    L"去:さ-る、キョ、コ",
    L"橋:はし、キョウ",
    L"業:ギョウ、わざ、ゴウ",
    L"曲:ま-げる、ま-がる、キョク、(くせ)",
    L"局:キョク、(つぼね)",
    L"銀:ギン、(しろがね)",
    L"区:ク",
    L"苦:くる-しい、にが-い、くる-しむ、くる-しめる、ク",
    L"具:グ、(そな-える)",
    L"君:きみ、クン",
    L"係:かかり、ケイ、かか-る",
    L"軽:かる-い、ケイ、かろ-やか",
    L"血:ち、ケツ",
    L"決:き-める、き-まる、ケツ",
    L"研:と-ぐ、ケン",
    L"県:ケン、(あがた)",
    L"庫:コ",
    L"湖:みずうみ、コ",
    L"向:む-かう、む-く、む-こう、コウ",
    L"幸:しあわ-せ、コウ、さち、さいわ-い",
    L"港:みなと、コウ",
    L"号:ゴウ",
    L"根:ね、コン",
    L"祭:まつ-り、まつ-る、サイ",
    L"皿:さら",
    L"仕:つか-える、シ",
    L"死:し-ぬ、シ",
    L"使:つか-う、シ",
    L"始:はじ-める、はじ-まる、シ",
    L"指:ゆび、さ-す、シ",
    L"歯:は、シ",
    L"詩:シ、(うた)",
    L"次:つぎ、つ-ぐ、ジ、シ",
    L"事:こと、ジ",
    L"持:も-つ、ジ",
    L"式:シキ",
    L"実:みの-る、み、ジツ",
    L"写:うつ-す、うつ-る、シャ",
    L"者:もの、シャ",
    L"主:あるじ、おも、ぬし、シュ",
    L"守:まも-る、シュ、ス、(も-り)",
    L"取:と-る、シュ",
    L"酒:さけ、シュ、さか",
    L"受:う-ける、う-かる、ジュ",
    L"州:シュウ、す",
    L"拾:ひろ-う、シュウ、ジュウ",
    L"終:お-わる、お-える、シュウ",
    L"習:なら-う、シュウ",
    L"集:あつ-める、あつ-まる、シュウ、つど-う",
    L"住:す-む、ジュウ、す-まう",
    L"重:おも-い、かさ-ねる、かさ-なる、ジュウ、チョウ",
    L"宿:やど、やど-る、シュク、やど-す",
    L"所:ところ、ショ",
    L"暑:あつ-い、ショ",
    L"助:たす-ける、たす-かる、ジョ",
    L"昭:ショウ",
    L"消:け-す、き-える、ショウ",
    L"商:あきな-い、ショウ",
    L"章:ショウ",
    L"勝:か-つ、ショウ、まさ-る",
    L"乗:の-る、の-せる、ジョウ",
    L"植:う-える、ショク",
    L"申:もう-す、シン、(さる)",
    L"身:み、シン",
    L"神:かみ、シン、ジン",
    L"真:シン、ま",
    L"深:ふか-い、シン、ふか-める、ふか-まる",
    L"進:すす-む、シン、すす-める",
    L"世:よ、セイ、セ",
    L"整:ととの-う、ととの-える、セイ",
    L"昔:むかし、シャク",
    L"全:すべ-て、ゼン、まった-く",
    L"相:ソウ、あい、ショウ",
    L"送:おく-る、ソウ",
    L"想:ソウ、(おも-う)",
    L"息:いき、ソク",
    L"速:はや-い、ソク、はや-める、はや-まる、すみ-やか",
    L"族:ゾク",
    L"他:ほか、タ",
    L"打:う-つ、ダ",
    L"対:タイ、ツイ",
    L"待:ま-つ、タイ",
    L"代:ダイ、タイ、か-える、か-わる、よ、しろ",
    L"第:ダイ",
    L"題:ダイ",
    L"炭:すみ、タン",
    L"短:みじか-い、タン",
    L"談:ダン",
    L"着:チャク、つ-ける、つ-く、き-せる、き-る、ジャク",
    L"注:そそ-ぐ、チュウ",
    L"柱:はしら、チュウ",
    L"丁:チョウ、テイ",
    L"帳:チョウ",
    L"調:しら-べる、チョウ、ととの-える、ととの-う",
    L"追:お-う、ツイ",
    L"定:さだ-める、さだ-まる、テイ、ジョウ",
    L"庭:にわ、テイ",
    L"笛:ふえ、テキ",
    L"鉄:テツ",
    L"転:ころ-ぶ、テン、ころ-がす、ころ-げる、ころ-がる",
    L"都:みやこ、ト、ツ",
    L"度:ド、たび、タク",
    L"投:な-げる、トウ",
    L"豆:まめ、トウ、ズ",
    L"島:しま、トウ",
    L"湯:ゆ、トウ",
    L"登:のぼ-る、トウ、ト",
    L"等:ひと-しい、など、トウ、ら",
    L"動:うご-く、うご-かす、ドウ",
    L"童:わらべ、ドウ",
    L"農:ノウ",
    L"波:なみ、ハ",
    L"配:くば-る、ハイ",
    L"倍:バイ",
    L"箱:はこ、ばこ",
    L"畑:はたけ、はた",
    L"発:ハツ",
    L"反:そ-る、そら-す、ハン、タン",
    L"坂:さか",
    L"板:いた、ハン、バン",
    L"皮:かわ、ヒ",
    L"悲:かな-しい、かな-しむ、ヒ",
    L"美:うつく-しい、ビ、ミ",
    L"鼻:はな、ビ",
    L"筆:ふで、ヒツ",
    L"氷:こおり、ヒョウ、ヒ",
    L"表:おもて、あらわ-す、あらわ-れる、ヒョウ",
    L"秒:ビョウ",
    L"病:やまい、ビョウ、や-む",
    L"品:しな、ヒン",
    L"負:ま-ける、ま-かす、お-う、フ",
    L"部:ブ",
    L"服:フク",
    L"福:フク",
    L"物:ブツ、もの、モツ",
    L"平:たい-ら、ひら、ヘイ、ビョウ",
    L"返:かえ-す、かえ-る、ヘン",
    L"勉:ベン",
    L"放:はな-つ、はな-す、ホウ、ほう-る",
    L"味:あじ、あじ-わう、ミ",
    L"命:いのち、メイ、ミョウ",
    L"面:メン、つら、(おも)、(おもて)",
    L"問:と-い、と-う、モン",
    L"役:ヤク、エキ",
    L"薬:くすり、ヤク",
    L"由:ユ、ユウ",
    L"油:あぶら、ユ",
    L"有:あ-る、ユウ",
    L"遊:あそ-ぶ、ユウ",
    L"予:ヨ",
    L"羊:ひつじ、ヨウ",
    L"洋:ヨウ",
    L"葉:は、ヨウ",
    L"陽:ヨウ、ひ",
    L"様:さま、ヨウ",
    L"落:お-とす、お-ちる、ラク",
    L"流:なが-す、なが-れる、リュウ",
    L"旅:たび、リョ",
    L"両:リョウ",
    L"緑:みどり、リョク",
    L"礼:レイ、ライ",
    L"列:レツ",
    L"練:レン、ね-る",
    L"路:ロ、じ",
    L"和:ワ、なご-む、なご-やか、やわ-らぐ、やわ-らげる、あ-える",
};

static const LPCWSTR g_aszMojiEnglish[200] =
{
    L"悪:Bad, Evil",
    L"安:Cheap, Safe, Peace",
    L"暗:Dark, Dim",
    L"医:Medical, Doctor",
    L"委:Committee, Entrust",
    L"意:Meaning, Intent",
    L"育:Education, Grow Up",
    L"員:Member, Staff",
    L"院:Institution, Hospital",
    L"飲:Drink, Swallow",
    L"運:Carry, Luck, Drive",
    L"泳:Swim",
    L"駅:Station",
    L"央:(Center)",
    L"横:Horizontal, Side, Crossing",
    L"屋:Shop, Roof",
    L"温:Warm, Temperature",
    L"化:Transform, (Chemistry)",
    L"荷:Luggage, Cargo, Load",
    L"界:World, Field",
    L"開:Open",
    L"階:Floor",
    L"寒:Cold",
    L"感:Feeling, Sense",
    L"漢:Old China, Kanji, Man",
    L"館:Hall, Mansion",
    L"岸:Shore, Coast",
    L"起:Wake Up, Happen",
    L"期:Period, Time, Opportunity",
    L"客:Customer, Guest",
    L"究:Research",
    L"急:Urgent",
    L"級:Grade",
    L"宮:Palace",
    L"球:Ball, Globe",
    L"去:Leaving, Previous",
    L"橋:Bridge",
    L"業:Business, Work, Karma",
    L"曲:Song, Bending",
    L"局:Bureau, Department, Office Lady",
    L"銀:Silver",
    L"区:Ward, Section, District",
    L"苦:Bitter, Painful, Weak Point",
    L"具:Ingredients, Tools",
    L"君:You",
    L"係:Person In Charge",
    L"軽:Light (the opposite of heavy)",
    L"血:Blood",
    L"決:Decide, Determination",
    L"研:Lab, Study, Training, Sharpen",
    L"県:Prefecture",
    L"庫:Repository, Vault, Warehouse",
    L"湖:Lake",
    L"向:Go Towards, Direction, Suitable",
    L"幸:Happy, Lucky",
    L"港:Port, Harbor",
    L"号:Number, Issue",
    L"根:Root",
    L"祭:Festival, Enshrine",
    L"皿:Dish, Plate",
    L"仕:Serve, Service",
    L"死:Death, Die",
    L"使:Use, Messenger",
    L"始:Begin",
    L"指:Finger, Pointing",
    L"歯:Teeth",
    L"詩:Poem",
    L"次:Next, (-th)",
    L"事:Thing, Case, Matter",
    L"持:Hold",
    L"式:Formula, Style, Ceremony, Type",
    L"実:Fruit, Truth, Reality, Nut",
    L"写:Photo, Copy",
    L"者:Person, Someone",
    L"主:Main, Master, Lord",
    L"守:Protect, Defend, Guard",
    L"取:Take, Pick Up",
    L"酒:Alcohol",
    L"受:Receive",
    L"州:State, Province, Sandbank",
    L"拾:Pick Up",
    L"終:Ending",
    L"習:Learn, Habit",
    L"集:Collection, Gather",
    L"住:Reside, Living",
    L"重:Heavy",
    L"宿:Inn, Stay",
    L"所:Place, Location",
    L"暑:Heat, Hot, Humid",
    L"助:Help, Assist, Rescue",
    L"昭:Showa Era",
    L"消:Erase, Disappear, Extinguish",
    L"商:Business, Quotient",
    L"章:Chapter",
    L"勝:Victory, Win",
    L"乗:Ride, -th power",
    L"植:Plant",
    L"申:Stating, (Monkey)",
    L"身:Body",
    L"神:God, Diety",
    L"真:True, Genuine",
    L"深:Deep",
    L"進:Move On, Progress, -base number",
    L"世:World, Society, Generation",
    L"整:Adjust",
    L"昔:Long Ago, Olden Days",
    L"全:All, Entirely",
    L"相:(Mutual), (Phase), (Aspect)",
    L"送:Send",
    L"想:Thoughts, Idea, Imagination",
    L"息:Breath",
    L"速:Fast, Quick",
    L"族:Group, Tribe",
    L"他:Other, Others",
    L"打:Hit, Strike",
    L"対:Pair, Versus",
    L"待:Wait",
    L"代:Generation, Era, Substitution",
    L"第:No., -th",
    L"題:Subject, Problem, Theme",
    L"炭:Charcoal",
    L"短:Short (of length)",
    L"談:Story, Talk",
    L"着:Arrival, Clothing",
    L"注:Note, Annotate, Minding, Pour",
    L"柱:Pillar",
    L"丁:Block, Cho (Japanese unit), Even number",
    L"帳:Account, Notebook",
    L"調:Tone, Investigate, Tune",
    L"追:Follow Up, Chase, Run After",
    L"定:Constant, Preset, Decide",
    L"庭:Garden, Yard",
    L"笛:Whistle, Flute",
    L"鉄:Iron",
    L"転:Fall Down, Roll",
    L"都:Capital",
    L"度:Degree, Time, Occasion",
    L"投:Throw",
    L"豆:Beans",
    L"島:Island",
    L"湯:Hot Water",
    L"登:Climb, Ascend, Register",
    L"等:Etc., Equal, Grade",
    L"動:Move, Motion",
    L"童:Child",
    L"農:Agriculture",
    L"波:Wave",
    L"配:Distribute, Deliver",
    L"倍:Doubled, Multiplied",
    L"箱:Box",
    L"畑:Field (of not rice)",
    L"発:Departure, Beginning",
    L"反:Anti-, Opposite",
    L"坂:Slope, Hill",
    L"板:Board, Plank",
    L"皮:Leather, Skin, Bark",
    L"悲:Sad",
    L"美:Beauty",
    L"鼻:Nose",
    L"筆:Writing Brush",
    L"氷:Ice",
    L"表:Table, Surface",
    L"秒:Seconds",
    L"病:Disease, Illness",
    L"品:Goods, Product",
    L"負:Negative, Minus, Lose, Bear",
    L"部:Part, Department, Category, Club",
    L"服:Clothes",
    L"福:Good Fortune",
    L"物:Object, Thing, Stuff",
    L"平:Flat, Broad",
    L"返:Return",
    L"勉:Exertion, Study",
    L"放:Release, Put Away",
    L"味:Taste, Flavor",
    L"命:Life, Command",
    L"面:Surface, Face, Side",
    L"問:Question, Asking",
    L"役:Role, Position, Service, Battle",
    L"薬:Drug, Medicine",
    L"由:Reason, Cause",
    L"油:Oil",
    L"有:Possess, There Is",
    L"遊:Play, Enjoy",
    L"予:Forecast",
    L"羊:Sheep",
    L"洋:Western, Sea",
    L"葉:Leaf",
    L"陽:Sunlight, (Positive)",
    L"様:Mr./Ms., Manner, Way",
    L"落:Fall, Drop",
    L"流:Flow, Stream, Style, Manner, Method",
    L"旅:Journey, Travel, Trip, Tour",
    L"両:Both, (Old Japanese currency)",
    L"緑:Green",
    L"礼:Courtesy, Etiquette, Manners",
    L"列:Column, Queue",
    L"練:Training, Knead",
    L"路:Road, Path",
    L"和:Sum, Harmony, Japanese style",
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

    ZeroMemory(g_ahbmKanji3, sizeof(g_ahbmKanji3));
    for (UINT j = 0; j < _countof(g_ahbmKanji3); ++j)
    {
        g_ahbmKanji3[j] = LoadBitmap(g_hInstance, MAKEINTRESOURCE(1000 + j));
        if (g_ahbmKanji3[j] == NULL)
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
    return (_countof(g_ahbmKanji3) + COLUMNS * ROWS - 1) / (COLUMNS * ROWS);
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

        for (j = 0; j < _countof(g_ahbmKanji3); ++j)
        {
            GetMojiRect(j, &rc);
            hbmOld = SelectObject(hdcMem, g_ahbmKanji3[g_map[j]]);
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

    const std::vector<GA>& v = g_kanji3_kakijun[g_nMoji];

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

    for (j = 0; j < _countof(g_ahbmKanji3); ++j)
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

    for (UINT j = 0; j < _countof(g_ahbmKanji3); ++j)
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
    for (i = 0; i < _countof(g_ahbmKanji3); ++i)
    {
        if (g_ahbmKanji3[i])
            DeleteObject(g_ahbmKanji3[i]);
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
