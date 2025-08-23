#pragma once

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
        Sleep(dwMilliseconds / 10);
    else
        Sleep(dwMilliseconds);
}

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
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_ABOUT, TEXT("About"));

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

static inline INT_PTR CALLBACK
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

BOOL SerializeRegion(std::vector<WORD>& out, HRGN hRgn);
HRGN DeserializeRegion(const WORD *pw, size_t size);
