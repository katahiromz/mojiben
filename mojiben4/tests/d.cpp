// Moji No Benkyo (3)
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

// Japanese, Shift_JIS
#include <windows.h>
#include <math.h>

static const TCHAR g_szCaption[] = "Win32 Template";
static const TCHAR g_szClassName[] = "Win32 Template";

HINSTANCE g_hInstance;
HWND g_hMainWnd;
INT g_k;

VOID OnPaint(HWND hWnd, HDC hdc)
{
    double cost, sint, cost2, sint2;
    POINT apt[3];
    double theta = g_k * M_PI / 180.0;
    double theta2 = (g_k + 20) * M_PI / 180.0;
    cost = std::cos(theta);
    sint = std::sin(theta);
    cost2 = std::cos(theta2);
    sint2 = std::sin(theta2);
    apt[0].x = 150 + 200 * cost;
    apt[0].y = 150 + 200 * sint;
    apt[1].x = 150 + 200 * cost2;
    apt[1].y = 150 + 200 * sint2;
    apt[2].x = 150;
    apt[2].y = 150;
    BeginPath(hdc);
    Polygon(hdc, apt, 3);
    EndPath(hdc);
    StrokeAndFillPath(hdc);
    char sz[124];
    wsprintf(sz, "%d", g_k);
    TextOut(hdc, 0, 0, sz, lstrlen(sz));
}

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    switch(uMsg)
    {
    case WM_CREATE:
        g_k = 0;
        SetTimer(hWnd, 999, 50, NULL);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        if (hdc != NULL)
        {
            OnPaint(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, TRUE);
        g_k += 20;
        if (g_k > 360)
            g_k = 0;
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 999);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
    MSG msg;
    BOOL f;
    
    g_hInstance = hInstance;
    wcx.cbSize          = sizeof(WNDCLASSEX);
    wcx.style           = 0;
    wcx.lpfnWndProc     = WindowProc;
    wcx.cbClsExtra      = 0;
    wcx.cbWndExtra      = 0;
    wcx.hInstance       = hInstance;
    wcx.hIcon           = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground   = (HBRUSH)(COLOR_3DFACE + 1);
    wcx.lpszMenuName    = NULL;
    wcx.lpszClassName   = g_szClassName;
    wcx.hIconSm         = (HICON)LoadImage(NULL, IDI_APPLICATION,
        IMAGE_ICON, 
        GetSystemMetrics(SM_CXSMICON), 
        GetSystemMetrics(SM_CYSMICON), 0);
    if (!RegisterClassEx(&wcx))
        return 1;
    
    g_hMainWnd = CreateWindow(g_szClassName, g_szCaption, 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 300, 300,
        NULL, NULL, hInstance, NULL);
    if (g_hMainWnd == NULL)
        return 2;

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
