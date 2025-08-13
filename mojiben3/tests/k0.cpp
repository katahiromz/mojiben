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

static char g_aszReadings[][64] =
{
    "0:ƒ[ƒA(‚ê‚¢)",
    "1:‚¢‚¿",
    "2:‚É",
    "3:‚³‚ñ",
    "4:‚æ‚ñ",
    "5:‚²",
    "6:‚ë‚­",
    "7:‚È‚È",
    "8:‚Í‚¿",
    "9:‚«‚ã‚¤",
    "10:‚¶‚ã‚¤",
    "20:‚É‚¶‚ã‚¤",
    "30:‚³‚ñ‚¶‚ã‚¤",
    "40:‚æ‚ñ‚¶‚ã‚¤",
    "50:‚²‚¶‚ã‚¤",
    "60:‚ë‚­‚¶‚ã‚¤",
    "70:‚È‚È‚¶‚ã‚¤",
    "80:‚Í‚¿‚¶‚ã‚¤",
    "90:‚«‚ã‚¤‚¶‚ã‚¤",
    "100:‚Ð‚á‚­",
    "200:‚É‚Ð‚á‚­",
    "300:‚³‚ñ‚Ñ‚á‚­",
    "400:‚æ‚ñ‚Ð‚á‚­",
    "500:‚²‚Ð‚á‚­",
    "600:‚ë‚Á‚Ò‚á‚­",
    "700:‚È‚È‚Ð‚á‚­",
    "800:‚Í‚Á‚Ò‚á‚­",
    "900:‚«‚ã‚¤‚Ð‚á‚­",
    "1000:‚¹‚ñ",
    "2000:‚É‚¹‚ñ",
    "3000:‚³‚ñ‚º‚ñ",
    "4000:‚æ‚ñ‚¹‚ñ",
    "5000:‚²‚¹‚ñ",
    "6000:‚ë‚­‚¹‚ñ",
    "7000:‚È‚È‚¹‚ñ",
    "8000:‚Í‚Á‚¹‚ñ",
    "9000:‚«‚ã‚¤‚¹‚ñ",
    "10000:‚¢‚¿‚Ü‚ñ",
    "20000:‚É‚Ü‚ñ",
    "30000:‚³‚ñ‚Ü‚ñ",
    NULL,
};

int main(void)
{
    HBITMAP hbm;
    HDC hdc;
    HGDIOBJ hbmOld;
    COLORREF clr;
    RECT rc;
    char sz3[64];
    char sz4[64];
    char *pch;
    INT i, j = 0;

#if 0
    j = 0;
#else
    for (j = 0; g_aszReadings[j]; ++j)
#endif
    {
        for (i = 0; i <= 0; i++)
        {
            wsprintf(sz3, "digit%03d-%02d.bmp", j, i);
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
            printf("static const BYTE digit_%03d_%02d[%ld] = {\n", j, i, cb);
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
