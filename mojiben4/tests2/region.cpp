extern HINSTANCE g_hInstance;

HRGN MyCreateRegion(INT res)
{
    HRSRC hRsrc = ::FindResource(g_hInstance, MAKEINTRESOURCE(res), RT_RCDATA);
    DWORD cbData = ::SizeofResource(g_hInstance, hRsrc);
    HGLOBAL hGlobal = ::LoadResource(g_hInstance, hRsrc);
    PVOID pvData = ::LockResource(hGlobal);
    return ::ExtCreateRegion(NULL, cbData, (RGNDATA *)pvData);
}
