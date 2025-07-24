#include <windows.h>
#include <stdio.h>

VOID PrintBinary(const BYTE *pb, DWORD cb)
{
    for(DWORD i = 0; i < cb; i++)
    {
        printf("0x%02X,", pb[i]);
        if (i % 16 == 15)
            printf("\n");
    }
}

int main(void)
{
    HBITMAP hbm;
    HDC hdc;
    HGDIOBJ hbmOld;
    COLORREF clr;
    RECT rc;
    char sz[] = "ˆø‰H‰_‰€‰“‰½‰È‰Ä‰Æ‰Ì‰æ‰ñ‰ïŠCŠGŠOŠpŠyŠˆŠÔŠÛŠâŠç‹D‹L‹A‹|‹‹›‹ž‹­‹³‹ßŒZŒ`ŒvŒ³Œ¾Œ´ŒËŒÃŒßŒãŒêHŒöLŒðŒõls‚‰©‡’J‘•¡Ë×ìŽZŽ~Žs–îŽoŽvŽ†Ž›Ž©ŽžŽºŽÐŽãŽñHTt‘­êFHSVe}”¼º¯°Øá‘Dü‘O‘g‘–‘½‘¾‘Ì‘ä’n’r’m’ƒ’‹’·’¹’©’¼’Ê’í“X“_“d““~“–“Œ“š“ª“¯“¹“Ç“à“ì“÷”n”„”ƒ”ž”¼”Ô•ƒ•—•ª•·•Ä•à•ê•û–k–ˆ–…–œ–¾–Â–Ñ–å–é–ì—F—p—j—ˆ—¢—˜b";
    char sz3[64];
    char sz4[64];
    char *pch;
    INT i, j = 0;

#if 1
    j = 85;
#else
    for (pch = sz; *pch; pch += 2)
#endif
    {
        for (i = 0; i < 32; i++)
        {
            wsprintf(sz3, "kanji%03d-%02d.bmp", j, i);
            hbm = (HBITMAP)LoadImage(NULL, sz3, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hbm == NULL)
                break;

            hdc = CreateCompatibleDC(NULL);
            hbmOld = SelectObject(hdc, hbm);
            HRGN hRgn, hRgn2;

            hRgn = CreateRectRgn(0, 0, 0, 0);
            for(int y = 0; y < 300; y++)
            {
                for(int x = 0; x < 300; x++)
                {
                    clr = GetPixel(hdc, x, y);
                    if (clr == RGB(0, 0, 0))
                    {
                        hRgn2 = CreateRectRgn(x, y, x + 1, y + 1);
                        CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
                        DeleteObject(hRgn2);
                    }
                }
            }

            DWORD cb = GetRegionData(hRgn, 0, NULL);
            LPBYTE pb = new BYTE[cb];
            GetRegionData(hRgn, cb, (RGNDATA *)pb);
            printf("static const BYTE kanji_%03d_%02d[%ld] = {\n", j, i, cb);
            PrintBinary(pb, cb);
            printf("};\n");
            if (cb & 0xF)
                printf("WARNING\n");
            delete[] pb;

            SelectObject(hdc, hbmOld);
        }
        ++j;
    }

    return 0;
}
