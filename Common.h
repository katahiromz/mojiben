#pragma once

#ifndef GET_SC_WPARAM
    #define GET_SC_WPARAM(wParam) ((INT)wParam & 0xFFF0)
#endif

#define SYSCOMMAND_ABOUT 0x3330
#define SYSCOMMAND_HIGH_SPEEED 0x3340

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

static inline void updateSystemMenu(HMENU hSysMenu)
{
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_HIGH_SPEEED, LoadStringDx(5));
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, SYSCOMMAND_ABOUT, LoadStringDx(2));
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
