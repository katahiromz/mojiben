#include <windows.h>

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, FAR * LPBITMAPINFOEX;

BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm)
{
    BOOL f;
    DWORD dwError;
    BITMAPFILEHEADER bf;
    BITMAPINFOEX bi;
    BITMAPINFOHEADER *pbmih;
    DWORD cb;
    DWORD cColors, cbColors;
    HDC hDC;
    HANDLE hFile;
    LPVOID pBits;
    BITMAP bm;

    if (!GetObject(hbm, sizeof(BITMAP), &bm))
        return FALSE;

    pbmih = &bi.bmiHeader;
    ZeroMemory(pbmih, sizeof(BITMAPINFOHEADER));
    pbmih->biSize             = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth            = bm.bmWidth;
    pbmih->biHeight           = bm.bmHeight;
    pbmih->biPlanes           = 1;
    pbmih->biBitCount         = bm.bmBitsPixel;
    pbmih->biCompression      = BI_RGB;
    pbmih->biSizeImage        = bm.bmWidthBytes * bm.bmHeight;

    if (bm.bmBitsPixel < 16)
        cColors = 1 << bm.bmBitsPixel;
    else
        cColors = 0;
    cbColors = cColors * sizeof(RGBQUAD);

    bf.bfType = 0x4d42;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    cb = sizeof(BITMAPFILEHEADER) + pbmih->biSize + cbColors;
    bf.bfOffBits = cb;
    bf.bfSize = cb + pbmih->biSizeImage;

    pBits = HeapAlloc(GetProcessHeap(), 0, pbmih->biSizeImage);
    if (pBits == NULL)
        return FALSE;

    f = FALSE;
    hDC = GetDC(NULL);
    if (hDC != NULL)
    {
        if (GetDIBits(hDC, hbm, 0, bm.bmHeight, pBits, (BITMAPINFO*)&bi,
            DIB_RGB_COLORS))
        {
            hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL |
                               FILE_FLAG_WRITE_THROUGH, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                f = WriteFile(hFile, &bf, sizeof(BITMAPFILEHEADER), &cb, NULL) &&
                    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &cb, NULL) &&
                    WriteFile(hFile, bi.bmiColors, cbColors, &cb, NULL) &&
                    WriteFile(hFile, pBits, pbmih->biSizeImage, &cb, NULL);
                if (!f)
                    dwError = GetLastError();
                CloseHandle(hFile);

                if (!f)
                    DeleteFile(pszFileName);
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
        ReleaseDC(NULL, hDC);
    }
    else
        dwError = GetLastError();

    HeapFree(GetProcessHeap(), 0, pBits);
    SetLastError(dwError);
    return f;
}

#define SIZE 300

int main(void)
{
    HBITMAP hbm;
    HDC hdc;
    HGDIOBJ hbmOld, hFontOld;
    HFONT hFont;
    LOGFONT lf;
    BITMAPINFO bi;
    LPVOID pvBits;
    char sz[] = "ˆ«ˆÀˆÃˆãˆÏˆÓˆçˆõ‰@ˆù‰^‰j‰w‰›‰¡‰®‰·‰»‰×ŠEŠJŠKŠ¦Š´Š¿ŠÙŠİ‹NŠú‹q‹†‹}‹‰‹{‹…‹‹´‹Æ‹È‹Ç‹â‹æ‹ê‹ïŒNŒWŒyŒŒŒˆŒ¤Œ§ŒÉŒÎŒüK`†ªÕMd€gnw•Ÿ–®ÀÊÒåçæğóBEIKWZdhŠ‹•ºÁ¤ÍŸæA\g_^[i¢®Ì‘S‘Š‘—‘z‘§‘¬‘°‘¼‘Å‘Î‘Ò‘ã‘æ‘è’Y’Z’k’…’’Œ’š’ ’²’Ç’è’ë“J“S“]“s“x“Š“¤“‡“’“o“™“®“¶”_”g”z”{” ”¨”­”½â”Â”ç”ß”ü•@•M•X•\•b•a•i•‰•”••Ÿ•¨•½•Ô•×•ú–¡–½–Ê–â–ğ–ò—R–û—L—V—\—r—m—t—z—l——¬—·—¼—Î—ç—ñ—û˜H˜a";
    char sz2[32];
    char *p;

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = SIZE;
    bi.bmiHeader.biHeight       = SIZE;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 24;
    
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -SIZE;
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lf.lfQuality = NONPROOF_QUALITY;
    lstrcpy(lf.lfFaceName, "ƒ‚ƒgƒ„‹³‰È‘3“™•");
    hFont = CreateFontIndirect(&lf);
    hdc = CreateCompatibleDC(NULL);
    hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    hFontOld = SelectObject(hdc, hFont);
    int count = 0;
    for(p = sz; *p; count++)
    {
        sz2[0] = *p++;
        sz2[1] = *p++;
        sz2[2] = 0;
        hbmOld = SelectObject(hdc, hbm);
        TextOut(hdc, 0, 0, sz2, 2);
        SelectObject(hdc, hbmOld);
        wsprintf(sz2, "kanji%03d.bmp", count);
        SaveBitmapToFile(sz2, hbm);
    }
    SelectObject(hdc, hFontOld);

    DeleteObject(hbm);
    DeleteObject(hFont);
    DeleteDC(hdc);
    return 0;
}
