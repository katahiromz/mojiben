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
    char sz[] = "ˆÝˆÙˆâˆæ‰F‰f‰„‰ˆ‰¶‰äŠDŠgŠvŠtŠ„Š”Š±ŠªŠÅŠÈŠëŠ÷Šö‹M‹^‹z‹Ÿ‹¹‹½‹Î‹ØŒnŒhŒxŒ€ŒƒŒŠŒ”Œ¦Œ Œ›Œ¹ŒµŒÈŒÄŒë@Fcg~|’œ¢»ÀÏÙôûŽ\ŽŠŽ„ŽpŽ‹ŽŒŽŽ¥ŽËŽÌŽÚŽáŽ÷Žû@AO]cknƒˆ”œ³«áöjm‚„¡·¹½ãéêòôõ‘K‘P‘t‘‹‘n‘•‘w‘€‘ ‘Ÿ‘¶‘¸‘Þ‘î’S’T’a’i’g’l’ˆ’‰’˜’¡’¸’°’ª’À’É“G“W“¢“}“œ“Í“ï“û”F”[”]”h”q”w”x”o”Ç”Ó”Û”á”é•U• •±•À•Ã•Â•Ð•â•é•ó–K–S–Y–_–‡–‹–§–¿–Í–ó—X—D—a—c—~—‚——‘——— —¥—Õ˜N˜_";
    char sz3[64];
    char sz4[64];
    char *pch;
    INT i, j = 0;

#if 1
    j = 146;
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
