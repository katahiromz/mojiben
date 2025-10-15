#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>

//#define PRINTF
#define PRINTF printf

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

HRGN MyCreateRegion(LPCVOID pvData, DWORD cbData)
{
    return ::ExtCreateRegion(NULL, cbData, (RGNDATA *)pvData);
}

// �����f�[�^�̈��S�Ȓǉ�
inline void SerializeRun(std::vector<WORD>& out, WORD start, WORD length)
{
    PRINTF("Run: start=%d, length=%d\n", start, length);
    out.push_back(start);
    out.push_back(length);
}

BOOL SerializeRegion(std::vector<WORD>& out, HRGN hRgn)
{
    RECT rc;
    if (GetRgnBox(hRgn, &rc) == NULLREGION)
        return FALSE;

    INT x = rc.left;
    INT y = rc.top;
    INT cx = rc.right - rc.left;
    INT cy = rc.bottom - rc.top;

    // �T�C�Y�����̏ꍇ��[���̏ꍇ�̃`�F�b�N
    if (cx <= 0 || cy <= 0)
        return FALSE;

    BOOL wide = (cx >= cy);
    BYTE signature = wide ? 'H' : 'V';

    // �w�b�_�[���
    out.push_back((WORD)signature);
    out.push_back((WORD)x);
    out.push_back((WORD)y);
    out.push_back((WORD)cx);
    out.push_back((WORD)cy);

    if (wide)
    {
        // �����X�L�����F�e�s������
        for (INT y0 = y; y0 < y + cy; ++y0)
        {
            INT runStart = -1;  // ���݂̃����̊J�n�ʒu

            for (INT x0 = x; x0 < x + cx; ++x0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // �V���������̊J�n
                    {
                        runStart = x0;
                    }
                }
                else
                {
                    if (runStart != -1)  // �����̏I��
                    {
                        SerializeRun(out, runStart - x, x0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // �s�̏I�[�Ń����������Ă���ꍇ
            if (runStart != -1)
            {
                SerializeRun(out, runStart - x, (x + cx) - runStart);
            }

            // �s�̏I���}�[�J�[
            out.push_back(0xFFFF);
        }
    }
    else
    {
        // �����X�L�����F�e�������
        for (INT x0 = x; x0 < x + cx; ++x0)
        {
            INT runStart = -1;  // ���݂̃����̊J�n�ʒu

            for (INT y0 = y; y0 < y + cy; ++y0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // �V���������̊J�n
                    {
                        runStart = y0;
                    }
                }
                else
                {
                    if (runStart != -1)  // �����̏I��
                    {
                        SerializeRun(out, runStart - y, y0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // ��̏I�[�Ń����������Ă���ꍇ
            if (runStart != -1)
            {
                SerializeRun(out, runStart - y, (y + cy) - runStart);
            }

            // ��̏I���}�[�J�[
            out.push_back(0xFFFF);
        }
    }

    return TRUE;
}

HRGN DeserializeRegion(const WORD *pw, size_t size)
{
    if (size < 5)
    {
        PRINTF("Error: Insufficient data size\n");
        return NULL;
    }

    WORD signature = pw[0];
    BOOL isHorizontal = (signature == 'H');
    size_t offset = 1;

    if (signature != 'H' && signature != 'V')
    {
        PRINTF("Error: Invalid signature\n");
        return NULL;
    }

    // ���W�ƃT�C�Y�̓ǂݎ��
    INT x = (INT)pw[offset];
    INT y = (INT)pw[offset + 1];
    INT cx = (INT)pw[offset + 2];
    INT cy = (INT)pw[offset + 3];
    offset += 4;

    PRINTF("Deserializing: signature=%c, bounds=(%d,%d,%dx%d)\n",
           signature, x, y, cx, cy);

    // �T�C�Y�̑Ó����`�F�b�N
    if (cx <= 0 || cy <= 0)
    {
        PRINTF("Error: Invalid region size\n");
        return NULL;
    }

    // RGNDATA�\���̂̏���
    RGNDATAHEADER header = { sizeof(header) };
    header.iType = RDH_RECTANGLES;
    SetRect(&header.rcBound, x, y, x + cx, y + cy);

    std::vector<RECT> rects;

    if (isHorizontal)
    {
        INT yCurrent = y;
        while (offset < size && yCurrent < y + cy)
        {
            // �e�s�̃����f�[�^������
            while (offset + 1 < size)
            {
                WORD value = pw[offset];

                // �s�I���}�[�J�[�̃`�F�b�N
                if (value == 0xFFFF)
                {
                    offset++;
                    break;
                }

                // �����̊J�n�ʒu�ƃ����O�X���擾
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pw[offset];
                DWORD runLength = pw[offset + 1];
                offset += 2;

                PRINTF("H-Run: start=%d, length=%d at y=%d\n", runStart, runLength, yCurrent);

                // ���E�`�F�b�N
                if (runStart + runLength > (DWORD)cx)
                {
                    PRINTF("Warning: Run exceeds horizontal bounds\n");
                    runLength = cx - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        x + (INT)runStart,
                        yCurrent,
                        x + (INT)runStart + (INT)runLength,
                        yCurrent + 1
                    };
                    rects.push_back(r);
                }
            }
            yCurrent++;
        }
    }
    else
    {
        INT xCurrent = x;
        while (offset < size && xCurrent < x + cx)
        {
            // �e��̃����f�[�^������
            while (offset + 1 < size)
            {
                WORD value = pw[offset];

                // ��I���}�[�J�[�̃`�F�b�N
                if (value == 0xFFFF)
                {
                    offset++;
                    break;
                }

                // �����̊J�n�ʒu�ƃ����O�X���擾
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pw[offset];
                DWORD runLength = pw[offset + 1];
                offset += 2;

                PRINTF("V-Run: start=%d, length=%d at x=%d\n", runStart, runLength, xCurrent);

                // ���E�`�F�b�N
                if (runStart + runLength > (DWORD)cy)
                {
                    PRINTF("Warning: Run exceeds vertical bounds\n");
                    runLength = cy - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        xCurrent,
                        y + (INT)runStart,
                        xCurrent + 1,
                        y + (INT)runStart + (INT)runLength
                    };
                    rects.push_back(r);
                }
            }
            xCurrent++;
        }
    }

    if (rects.empty())
    {
        PRINTF("Warning: No rectangles generated\n");
        return NULL;
    }

    // RECT�����Ƀ��[�W�������쐬
    header.nCount = (DWORD)rects.size();
    size_t dataSize = sizeof(RGNDATAHEADER) + rects.size() * sizeof(RECT);
    RGNDATA* rgndata = (RGNDATA*)malloc(dataSize);

    if (!rgndata)
    {
        PRINTF("Error: Memory allocation failed\n");
        return NULL;
    }

    memcpy(&rgndata->rdh, &header, sizeof(RGNDATAHEADER));
    memcpy(rgndata->Buffer, &rects[0], rects.size() * sizeof(RECT));

    HRGN hRgn = ExtCreateRegion(NULL, (DWORD)dataSize, rgndata);
    free(rgndata);

    PRINTF("Created region with %u rectangles\n", (UINT)rects.size());
    return hRgn;
}

// �����f�[�^�̈��S�Ȓǉ� (254��)
inline void SerializeRun254(std::vector<BYTE>& out, BYTE start, BYTE length)
{
    PRINTF("Run: start=%d, length=%d\n", start, length);
    out.push_back(start);
    out.push_back(length);
}

BOOL SerializeRegion254(std::vector<BYTE>& out, HRGN hRgn)
{
    RECT rc;
    if (GetRgnBox(hRgn, &rc) == NULLREGION)
        return FALSE;

    INT x = rc.left;
    INT y = rc.top;
    INT cx = rc.right - rc.left;
    INT cy = rc.bottom - rc.top;

    // �T�C�Y�����̏ꍇ��[���̏ꍇ�̃`�F�b�N
    if (cx <= 0 || cy <= 0 || cx > 254 || cy > 254)
    {
        return FALSE;
    }
    if (x < 0 || y < 0 || x > 254 || y > 254)
    {
        return FALSE;
    }

    BOOL wide = (cx >= cy);
    BYTE signature = wide ? 'h' : 'v';

    // �w�b�_�[���
    out.push_back((BYTE)signature);
    out.push_back((BYTE)x);
    out.push_back((BYTE)y);
    out.push_back((BYTE)cx);
    out.push_back((BYTE)cy);

    if (wide)
    {
        // �����X�L�����F�e�s������
        for (INT y0 = y; y0 < y + cy; ++y0)
        {
            INT runStart = -1;  // ���݂̃����̊J�n�ʒu

            for (INT x0 = x; x0 < x + cx; ++x0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // �V���������̊J�n
                    {
                        runStart = x0;
                    }
                }
                else
                {
                    if (runStart != -1)  // �����̏I��
                    {
                        SerializeRun254(out, runStart - x, x0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // �s�̏I�[�Ń����������Ă���ꍇ
            if (runStart != -1)
            {
                SerializeRun254(out, runStart - x, (x + cx) - runStart);
            }

            // �s�̏I���}�[�J�[
            out.push_back(0xFF);
        }
    }
    else
    {
        // �����X�L�����F�e�������
        for (INT x0 = x; x0 < x + cx; ++x0)
        {
            INT runStart = -1;  // ���݂̃����̊J�n�ʒu

            for (INT y0 = y; y0 < y + cy; ++y0)
            {
                BOOL inside = PtInRegion(hRgn, x0, y0);

                if (inside)
                {
                    if (runStart == -1)  // �V���������̊J�n
                    {
                        runStart = y0;
                    }
                }
                else
                {
                    if (runStart != -1)  // �����̏I��
                    {
                        SerializeRun254(out, runStart - y, y0 - runStart);
                        runStart = -1;
                    }
                }
            }

            // ��̏I�[�Ń����������Ă���ꍇ
            if (runStart != -1)
            {
                SerializeRun254(out, runStart - y, (y + cy) - runStart);
            }

            // ��̏I���}�[�J�[
            out.push_back(0xFF);
        }
    }

    return TRUE;
}

HRGN DeserializeRegion254(const BYTE *pb, size_t size)
{
    if (size < 5)
    {
        PRINTF("Error: Insufficient data size\n");
        return NULL;
    }

    BYTE signature = pb[0];
    BOOL isHorizontal = (signature == 'h');
    size_t offset = 1;

    if (signature != 'h' && signature != 'v')
    {
        PRINTF("Error: Invalid signature\n");
        return NULL;
    }

    // ���W�ƃT�C�Y�̓ǂݎ��
    INT x = (INT)pb[offset];
    INT y = (INT)pb[offset + 1];
    INT cx = (INT)pb[offset + 2];
    INT cy = (INT)pb[offset + 3];
    offset += 4;

    PRINTF("Deserializing: signature=%c, bounds=(%d,%d,%dx%d)\n",
           signature, x, y, cx, cy);

    // �T�C�Y�̑Ó����`�F�b�N
    if (cx <= 0 || cy <= 0)
    {
        PRINTF("Error: Invalid region size\n");
        return NULL;
    }

    // RGNDATA�\���̂̏���
    RGNDATAHEADER header = { sizeof(header) };
    header.iType = RDH_RECTANGLES;
    SetRect(&header.rcBound, x, y, x + cx, y + cy);

    std::vector<RECT> rects;

    if (isHorizontal)
    {
        INT yCurrent = y;
        while (offset < size && yCurrent < y + cy)
        {
            // �e�s�̃����f�[�^������
            while (offset + 1 < size)
            {
                BYTE value = pb[offset];

                // �s�I���}�[�J�[�̃`�F�b�N
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // �����̊J�n�ʒu�ƃ����O�X���擾
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pb[offset];
                DWORD runLength = pb[offset + 1];
                offset += 2;

                PRINTF("H-Run: start=%d, length=%d at y=%d\n", runStart, runLength, yCurrent);

                // ���E�`�F�b�N
                if (runStart + runLength > (DWORD)cx)
                {
                    PRINTF("Warning: Run exceeds horizontal bounds\n");
                    runLength = cx - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        x + (INT)runStart,
                        yCurrent,
                        x + (INT)runStart + (INT)runLength,
                        yCurrent + 1
                    };
                    rects.push_back(r);
                }
            }
            yCurrent++;
        }
    }
    else
    {
        INT xCurrent = x;
        while (offset < size && xCurrent < x + cx)
        {
            // �e��̃����f�[�^������
            while (offset + 1 < size)
            {
                BYTE value = pb[offset];

                // ��I���}�[�J�[�̃`�F�b�N
                if (value == 0xFF)
                {
                    offset++;
                    break;
                }

                // �����̊J�n�ʒu�ƃ����O�X���擾
                if (offset + 1 >= size)
                {
                    PRINTF("Error: Unexpected end of data\n");
                    return NULL;
                }

                DWORD runStart = pb[offset];
                DWORD runLength = pb[offset + 1];
                offset += 2;

                PRINTF("V-Run: start=%d, length=%d at x=%d\n", runStart, runLength, xCurrent);

                // ���E�`�F�b�N
                if (runStart + runLength > (DWORD)cy)
                {
                    PRINTF("Warning: Run exceeds vertical bounds\n");
                    runLength = cy - runStart;
                }

                if (runLength > 0)
                {
                    RECT r = {
                        xCurrent,
                        y + (INT)runStart,
                        xCurrent + 1,
                        y + (INT)runStart + (INT)runLength
                    };
                    rects.push_back(r);
                }
            }
            xCurrent++;
        }
    }

    if (rects.empty())
    {
        PRINTF("Warning: No rectangles generated\n");
        return NULL;
    }

    // RECT�����Ƀ��[�W�������쐬
    header.nCount = (DWORD)rects.size();
    size_t dataSize = sizeof(RGNDATAHEADER) + rects.size() * sizeof(RECT);
    RGNDATA* rgndata = (RGNDATA*)malloc(dataSize);

    if (!rgndata)
    {
        PRINTF("Error: Memory allocation failed\n");
        return NULL;
    }

    memcpy(&rgndata->rdh, &header, sizeof(RGNDATAHEADER));
    memcpy(rgndata->Buffer, &rects[0], rects.size() * sizeof(RECT));

    HRGN hRgn = ExtCreateRegion(NULL, (DWORD)dataSize, rgndata);
    free(rgndata);

    PRINTF("Created region with %u rectangles\n", (UINT)rects.size());
    return hRgn;
}

int main(void)
{
    WIN32_FIND_DATA find;
    HANDLE hFind = FindFirstFileA("RCData_*.bin.bmp.bmp", &find);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        PRINTF("No region data\n");
        return 1;
    }

    static char s_buf[60000];
    HDC hdc = CreateCompatibleDC(NULL);
    do
    {
        PRINTF("\n=== Processing: %s ===\n", find.cFileName);

        HBITMAP hbm = (HBITMAP)LoadImage(NULL, find.cFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (!hbm)
        {
            PRINTF("Error: Cannot open file\n");
            continue;
        }

        HRGN hRgn4 = CreateRectRgn(0, 0, 0, 0);

        HGDIOBJ hbmOld = SelectObject(hdc, hbm);
        for(int y = 0; y < 254; y++)
        {
            for(int x = 0; x < 254; x++)
            {
                if (!GetPixel(hdc, x, y))
                {
                    HRGN hRgn3 = CreateRectRgn(x, y, x + 1, y + 1);
                    CombineRgn(hRgn4, hRgn4, hRgn3, RGN_OR);
                    DeleteObject(hRgn3);
                }
            }
        }
        SelectObject(hdc, hbmOld);

        std::vector<BYTE> out;
        if (!SerializeRegion254(out, hRgn4))
        {
            puts("error");
            continue;
        }

        std::string name = find.cFileName;
        name += ".dat";

        FILE *fp = fopen(name.c_str(), "wb");
        fwrite(&out[0], out.size(), 1, fp);
        fclose(fp);

        DeleteObject(hRgn4);
    } while (FindNextFileA(hFind, &find));
    DeleteDC(hdc);

    FindClose(hFind);
    return 0;
}
