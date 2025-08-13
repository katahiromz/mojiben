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
    "0:�[���A(�ꂢ)",
    "1:����",
    "2:��",
    "3:����",
    "4:���",
    "5:��",
    "6:�낭",
    "7:�Ȃ�",
    "8:�͂�",
    "9:���イ",
    "10:���イ",
    "20:�ɂ��イ",
    "30:���񂶂イ",
    "40:��񂶂イ",
    "50:�����イ",
    "60:�낭���イ",
    "70:�ȂȂ��イ",
    "80:�͂����イ",
    "90:���イ���イ",
    "100:�ЂႭ",
    "200:�ɂЂႭ",
    "300:����тႭ",
    "400:���ЂႭ",
    "500:���ЂႭ",
    "600:����҂Ⴍ",
    "700:�ȂȂЂႭ",
    "800:�͂��҂Ⴍ",
    "900:���イ�ЂႭ",
    "1000:����",
    "2000:�ɂ���",
    "3000:���񂺂�",
    "4000:��񂹂�",
    "5000:������",
    "6000:�낭����",
    "7000:�ȂȂ���",
    "8000:�͂�����",
    "9000:���イ����",
    "10000:�����܂�",
    "20000:�ɂ܂�",
    "30000:����܂�",
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
