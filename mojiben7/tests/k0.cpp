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
    char sz[] = "ˆ¤ˆÄˆÈˆßˆÊˆïˆó‰p‰h•Q‰–‰ª‰­‰Á‰Ê‰Ý‰Û‰è‰ê‰üŠBŠQŠXŠeŠoŠƒŠ®Š¯ŠÇŠÖŠÏŠèŠòŠó‹GŠøŠí‹@‹c‹‹ƒ‹‹‹“‹™‹¤‹¦‹¾‹£‹ÉŒFŒPŒRŒSŒQŒaŒiŒ|Œ‡Œ‹ŒšŒ’Œ±ŒÅŒ÷DŒóN²·ØÅéÞèðŽDüŽ@ŽQŽYŽUŽcŽŽiŽŽŽ™Ž¡Ž Ž«Ž­Ž¸ŽØŽíŽüj‡‰¼Î¥ÄÆé“êbMˆä¬È´ÃÈÏÜßàóí‘I‘R‘ˆ‘q‘ƒ‘©‘¤‘±‘²‘·‘Ñ‘à’B’P’u’‡‰«’›’á’ê“I“T“`“k“w“”“­“Á“¿“È“Þ—œ”M”O”s”~”Žã”Ñ”ò•K•[•W•s•v•t•{•Œ•x•›•º•Ê•Ó•Ï•Ö•ï–@–]–q–––ž–¢–¯–³–ñ—E—v—{——˜—¤—Ç—¿—Ê—Ö—Þ—ß—â—á˜A˜V˜J˜^";
    char sz3[64];
    char sz4[64];
    char *pch;
    INT i, j = 0;

#if 1
    j = 166;
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
