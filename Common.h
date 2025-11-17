#pragma once

#include <olectl.h>
#include <shlwapi.h>
#include <stdio.h>
#include <tchar.h>

#ifndef GET_SC_WPARAM
    #define GET_SC_WPARAM(wParam) ((INT)wParam & 0xFFF0)
#endif

#define SYSCOMMAND_ABOUT 0x3330
#define SYSCOMMAND_HIGH_SPEEED 0x3340
#define SYSCOMMAND_STUDY_USING_JAPANESE 0x3350
#define SYSCOMMAND_STUDY_USING_ENGLISH 0x3360

enum STUDY_MODE
{
    STUDY_MODE_DEFAULT = 0,
    STUDY_MODE_USING_ENGLISH = -1,
    STUDY_MODE_USING_JAPANESE = +1,
};

extern HINSTANCE g_hInstance;
extern BOOL g_bHighSpeed;

static inline LPTSTR LoadStringDx(INT ids)
{
    static TCHAR sz[512];
    LoadString(g_hInstance, ids, sz, 512);
    return sz;
}

static inline void DoSleep(DWORD dwMilliseconds)
{
    if (g_bHighSpeed)
        Sleep(dwMilliseconds / 5);
    else
        Sleep(dwMilliseconds);
}
#define DO_SLEEP DoSleep

STUDY_MODE getStudyMode(VOID);
STUDY_MODE getStudyModeDefault(VOID);

static inline void updateSystemMenu(HWND hwnd)
{
    GetSystemMenu(hwnd, TRUE);
    HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);

    HMENU hChildMenu = CreatePopupMenu();
    InsertMenu(hChildMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_STUDY_USING_JAPANESE, L"Study using Japanese");
    InsertMenu(hChildMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_STUDY_USING_ENGLISH, L"Study using English");

    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_HIGH_SPEEED, TEXT("High-speed mode"));
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hChildMenu, TEXT("Study mode"));
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_ABOUT, LoadStringDx(2));

    ::CheckMenuItem(hSysMenu, SYSCOMMAND_HIGH_SPEEED, (g_bHighSpeed ? MF_CHECKED : MF_UNCHECKED));

    STUDY_MODE studyMode = getStudyMode();
    BOOL bRetry = FALSE;

retry:
    switch (studyMode)
    {
    case STUDY_MODE_USING_ENGLISH:
        ::CheckMenuRadioItem(hSysMenu, SYSCOMMAND_STUDY_USING_JAPANESE, SYSCOMMAND_STUDY_USING_ENGLISH,
                             SYSCOMMAND_STUDY_USING_ENGLISH, MF_BYCOMMAND);
        break;
    case STUDY_MODE_USING_JAPANESE:
        ::CheckMenuRadioItem(hSysMenu, SYSCOMMAND_STUDY_USING_JAPANESE, SYSCOMMAND_STUDY_USING_ENGLISH,
                             SYSCOMMAND_STUDY_USING_JAPANESE, MF_BYCOMMAND);
        break;
    case STUDY_MODE_DEFAULT:
    default:
        studyMode = getStudyModeDefault();
        if (!bRetry)
        {
            bRetry = TRUE;
            goto retry;
        }
    }
}

typedef LANGID (WINAPI *FN_GetThreadUILanguage)(VOID);
typedef LANGID (WINAPI *FN_SetThreadUILanguage)(LANGID);

static inline BOOL WonIsWindowsVistaOrGreater(VOID)
{
    OSVERSIONINFO osvi = { sizeof(osvi) };

    if (!GetVersionEx(&osvi))
        return FALSE;

    return osvi.dwMajorVersion >= 6;
}

static inline LANGID WonGetThreadUILanguage()
{
    static FN_GetThreadUILanguage s_fn = NULL;

    if (!s_fn)
        s_fn = reinterpret_cast<FN_GetThreadUILanguage>(
            GetProcAddress(GetModuleHandleA("kernel32"), "GetThreadUILanguage"));

    if (WonIsWindowsVistaOrGreater() && s_fn)
        return (*s_fn)();

    return LANGIDFROMLCID(GetThreadLocale());
}

static inline LANGID WonSetThreadUILanguage(LANGID LangID)
{
    static FN_SetThreadUILanguage s_fn = NULL;

    if (!s_fn)
        s_fn = reinterpret_cast<FN_SetThreadUILanguage>(
            GetProcAddress(GetModuleHandleA("kernel32"), "SetThreadUILanguage"));

    if (WonIsWindowsVistaOrGreater() && s_fn)
        return (*s_fn)(LangID);

    if (SetThreadLocale(MAKELCID(LangID, SORT_DEFAULT)))
        return LangID;

    return 0;
}

static inline STUDY_MODE
getStudyModeDefault(VOID)
{
    if (PRIMARYLANGID(WonGetThreadUILanguage()) == LANG_JAPANESE)
        return STUDY_MODE_USING_JAPANESE;
    return STUDY_MODE_USING_ENGLISH;
}

static inline STUDY_MODE
getStudyMode(VOID)
{
    HKEY hKey;
    LSTATUS error;
    error = RegOpenKeyEx(HKEY_CURRENT_USER,
                         TEXT("Software\\Katayama Hirofumi MZ\\Moji No Benkyou"),
                         0, KEY_READ, &hKey);
    if (error)
        return STUDY_MODE_DEFAULT;

    DWORD dwValue = 0;
    DWORD cbValue = sizeof(dwValue);
    error = RegQueryValueEx(hKey, TEXT("StudyMode"), NULL, NULL, (PBYTE)&dwValue, &cbValue);
    RegCloseKey(hKey);

    if (error)
        return STUDY_MODE_DEFAULT;

    switch (dwValue)
    {
    case STUDY_MODE_USING_ENGLISH:
    case STUDY_MODE_USING_JAPANESE:
        return (STUDY_MODE)dwValue;
    default:
        return STUDY_MODE_DEFAULT;
    }
}

static inline BOOL
rememberStudyMode(HWND hwnd, STUDY_MODE mode)
{
    HKEY hKey;
    LSTATUS error;
    DWORD dwDisposition;
    error = RegCreateKeyEx(HKEY_CURRENT_USER,
                           TEXT("Software\\Katayama Hirofumi MZ\\Moji No Benkyou"),
                           0, NULL, 0, KEY_WRITE, NULL, &hKey, &dwDisposition);
    if (error)
        return FALSE;

    if (mode == STUDY_MODE_DEFAULT)
    {
        error = RegDeleteValue(hKey, TEXT("StudyMode"));
    }
    else
    {
        DWORD dwValue = (DWORD)mode;
        error = RegSetValueEx(hKey, TEXT("StudyMode"), 0, REG_DWORD, (PBYTE)&dwValue, sizeof(dwValue));
    }

    RegCloseKey(hKey);

    if (!error)
        MessageBox(hwnd, TEXT("This mode change will take effect after restarting the app."), TEXT("Moji No Benkyou"), MB_ICONINFORMATION);

    return !error;
}

static inline BOOL
applyStudyMode(STUDY_MODE mode)
{
    switch (mode)
    {
    case STUDY_MODE_DEFAULT:
        break;
    case STUDY_MODE_USING_ENGLISH:
        WonSetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
        break;
    case STUDY_MODE_USING_JAPANESE:
        WonSetThreadUILanguage(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT));
        break;
    }

    return TRUE;
}

static inline
BOOL CopyText(HWND hwnd, LPCWSTR text)
{
    BOOL ret = FALSE;
    if (OpenClipboard(hwnd))
    {
        EmptyClipboard();
        SIZE_T cbText = (lstrlenW(text) + 1) * sizeof(WCHAR);
        HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, cbText);
        if (hGlobal)
        {
            LPWSTR psz = (LPWSTR)GlobalLock(hGlobal);
            if (psz)
            {
                CopyMemory(psz, text, cbText);
                GlobalUnlock(hGlobal);

                SetClipboardData(CF_UNICODETEXT, hGlobal);
                ret = TRUE;
            }
        }
        CloseClipboard();
    }
    return ret;
}

static inline BOOL
smartGetTextExtent(HDC hDC, LPCTSTR text, LONG maxWidth, LPSIZE pSize)
{
    SIZE siz;
    GetTextExtentPoint32(hDC, text, lstrlen(text), &siz);
    if (siz.cx <= maxWidth)
    {
        *pSize = siz;
        return FALSE; // Single line
    }

    RECT rc = { 0, 0, maxWidth, 0 };
    DrawText(hDC, text, -1, &rc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;
    *pSize = siz;
    return TRUE; // Multiple line
}

static inline BOOL
smartDrawText(HDC hDC, LPCTSTR text, LPRECT prc, INT maxWidth)
{
    SIZE siz;
    BOOL multiline = smartGetTextExtent(hDC, text, maxWidth, &siz);

    prc->right = prc->left + siz.cx;
    prc->bottom = prc->top + siz.cy;

    if (multiline)
        DrawText(hDC, text, -1, prc, DT_CENTER | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
    else
        DrawText(hDC, text, -1, prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_WORDBREAK);
    return TRUE;
}

BOOL SerializeRegion(std::vector<WORD>& out, HRGN hRgn);
HRGN DeserializeRegion(const WORD *pw, size_t data_size);

BOOL SerializeRegion254(std::vector<BYTE>& out, HRGN hRgn);
HRGN DeserializeRegion254(const BYTE *pb, size_t data_size);

HBITMAP LoadBitmapFromFile(LPCTSTR pszFileName);
BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm);

HBITMAP CreateBitmapFromRegionGeneric(HRGN hRgn, INT cxy);
HBITMAP CreateBitmapFromRegion(HRGN hRgn);
HBITMAP CreateBitmapFromRegion254(HRGN hRgn);

HRGN CreateRegionFromBitmapGeneric(HBITMAP hbm, INT cxy);
HRGN CreateRegionFromBitmap(HBITMAP hbm);
HRGN CreateRegionFromBitmap254(HBITMAP hbm);

static HBITMAP
LoadGif(HINSTANCE hInst, INT res)
{
    HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(res), TEXT("GIF"));
    DWORD cbData = ::SizeofResource(hInst, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(hInst, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    if (!pvData)
        return FALSE;

    IStream *pStream = SHCreateMemStream((PBYTE)pvData, cbData);
    if (!pStream)
        return FALSE;

    IPicture *pPicture;
    HRESULT hr = OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (void**)&pPicture);
    if (FAILED(hr))
    {
        pStream->Release();
        return FALSE;
    }

    OLE_HANDLE hPic = NULL;
    pPicture->get_Handle(&hPic);
    HBITMAP hBitmap = (HBITMAP)CopyImage((HBITMAP)hPic, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    pPicture->Release();
    pStream->Release();
    return hBitmap;
}

// スレッドの優先度を変更する。
class AutoPriority
{
public:
    AutoPriority()
    {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    }
    ~AutoPriority()
    {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    }
};

static BOOL
MyPlaySound(LPCTSTR pszName)
{
#if 1
    TCHAR szFile[MAX_PATH], szCommand[MAX_PATH + 64];
    {
        AutoPriority auto_priority;
        HRSRC hRsrc = ::FindResource(g_hInstance, pszName, TEXT("MP3"));
        DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
        HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
        PVOID pvData = ::LockResource(hGlobal);
        if (!pvData)
            return FALSE;

        TCHAR szTempPath[MAX_PATH];
        GetTempPath(_countof(szTempPath), szTempPath);
        GetTempFileName(szTempPath, TEXT("MJB"), 0, szFile);

        FILE *fout = _tfopen(szFile, TEXT("wb"));
        if (!fout)
            return FALSE;
        fwrite(pvData, cbData, 1, fout);
        fclose(fout);

        wnsprintf(szCommand, _countof(szCommand), TEXT("open \"%s\" type mpegvideo alias myaudio"), szFile);
    }

    mciSendString(szCommand, NULL, 0, 0);
    mciSendString(TEXT("play myaudio wait"), NULL, 0, 0);
    mciSendString(TEXT("close myaudio"), NULL, 0, 0);
    DeleteFile(szFile);
    return TRUE;
#else
    return PlaySound(pszName, g_hInstance, SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
#endif
}

static inline
unsigned __stdcall MyPlaySoundAsyncThreadProc(void *arg)
{
    LPCTSTR pszName = (LPCTSTR)arg;
    MyPlaySound(pszName);
    return 0;
}

static inline BOOL
MyPlaySoundAsync(LPCTSTR pszName)
{
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, MyPlaySoundAsyncThreadProc, (void *)pszName, 0, NULL);
    CloseHandle(hThread);
    return hThread != NULL;
}

static RECT workarea_from_window(HWND hwnd)
{
#if (WINVER >= 0x0500)
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (GetMonitorInfo(hMonitor, &mi))
    {
        return mi.rcWork;
    }
#endif
    RECT rc;
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    return rc;
}

static void reposition_point(LPPOINT ppt, SIZE siz, LPCRECT prc)
{
    if (ppt->x + siz.cx > prc->right)
        ppt->x = prc->right - siz.cx;
    if (ppt->y + siz.cy > prc->bottom)
        ppt->y = prc->bottom - siz.cy;
    if (ppt->x < prc->left)
        ppt->x = prc->left;
    if (ppt->y < prc->top)
        ppt->y = prc->top;
}

static void center_window(HWND hwnd)
{
    BOOL bChild = !!(GetWindowStyle(hwnd) & WS_CHILD);

    HWND hwndParent;
    if (bChild)
        hwndParent = ::GetParent(hwnd);
    else
        hwndParent = ::GetWindow(hwnd, GW_OWNER);

    RECT rcWorkArea = workarea_from_window(hwnd);

    RECT rcParent;
    if (hwndParent)
        ::GetWindowRect(hwndParent, &rcParent);
    else
        rcParent = rcWorkArea;

    SIZE sizParent = { 
        rcParent.right - rcParent.left, 
        rcParent.bottom - rcParent.top
    };

    RECT rc;
    ::GetWindowRect(hwnd, &rc);
    SIZE siz = { rc.right - rc.left, rc.bottom - rc.top };

    POINT pt;
    pt.x = rcParent.left + (sizParent.cx - siz.cx) / 2;
    pt.y = rcParent.top + (sizParent.cy - siz.cy) / 2;

    if (bChild && hwndParent)
    {
        ::GetClientRect(hwndParent, &rcParent);
        ::MapWindowPoints(hwndParent, NULL, (LPPOINT)&rcParent, 2);
        reposition_point(&pt, siz, &rcParent);

        ::ScreenToClient(hwndParent, &pt);
    }
    else
    {
        reposition_point(&pt, siz, &rcWorkArea);
    }

    ::SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static inline INT_PTR CALLBACK
AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        center_window(hDlg);
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

static inline LPVOID MyLoadRes(HINSTANCE hInst, LPCWSTR type, LPCWSTR name, DWORD *psize) {
    HRSRC hRsrc = ::FindResource(hInst, name, type);
    if (psize)
        *psize = ::SizeofResource(hInst, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    return ::LockResource(hGlobal);
}
