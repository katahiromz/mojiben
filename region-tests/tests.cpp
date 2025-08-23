#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>

//#define PRINTF
#define PRINTF printf

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

int main(void)
{
    WIN32_FIND_DATA find;
    HANDLE hFind = FindFirstFileA("REGION_*.bin", &find);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        PRINTF("No region data\n");
        return 1;
    }

    static char s_buf[60000];
    do
    {
        PRINTF("\n=== Processing: %s ===\n", find.cFileName);

        FILE *fp = fopen(find.cFileName, "rb");
        if (!fp)
        {
            PRINTF("Error: Cannot open file\n");
            continue;
        }

        size_t size = fread(s_buf, 1, sizeof(s_buf), fp);
        fclose(fp);

        PRINTF("File size: %u bytes\n", (UINT)size);

        HRGN hRgn1 = MyCreateRegion(s_buf, DWORD(size));
        if (hRgn1 == NULL)
        {
            PRINTF("Error: Invalid region data in %s\n", find.cFileName);
            continue;
        }

        std::vector<WORD> data;
        if (!SerializeRegion(data, hRgn1))
        {
            PRINTF("Error: Serialization failed for %s\n", find.cFileName);
            DeleteObject(hRgn1);
            continue;
        }

        PRINTF("Serialized data size: %u WORDs\n", (UINT)data.size());
        if (data.size() < 20)  // �����ȃf�[�^�̂ݕ\��
        {
            PRINTF("Serialized data: ");
            for (size_t iw = 0; iw < data.size(); ++iw)
                PRINTF("%04X ", data[iw]);
            PRINTF("\n");
        }

        HRGN hRgn2 = DeserializeRegion(&data[0], data.size());
        if (!hRgn2)
        {
            PRINTF("Error: Deserialization failed for %s\n", find.cFileName);
            DeleteObject(hRgn1);
            continue;
        }

        RECT rc1, rc2;
        GetRgnBox(hRgn1, &rc1);
        GetRgnBox(hRgn2, &rc2);

        PRINTF("Original bounds: { %ld, %ld, %ld, %ld }\n",
               rc1.left, rc1.top, rc1.right, rc1.bottom);
        PRINTF("Restored bounds:  { %ld, %ld, %ld, %ld }\n",
               rc2.left, rc2.top, rc2.right, rc2.bottom);

        if (!EqualRect(&rc1, &rc2))
        {
            PRINTF("WARNING: Bounding rectangles differ!\n");
        }

        if (!EqualRgn(hRgn1, hRgn2))
        {
            PRINTF("ERROR: Regions are not equal: %s\n", find.cFileName);
        }
        else
        {
            PRINTF("SUCCESS: Regions match perfectly!\n");
        }

        double compressionRatio = 100.0 * data.size() * sizeof(WORD) / (double)size;
        PRINTF("Compression ratio: %.2f%%\n", compressionRatio);

        DeleteObject(hRgn1);
        DeleteObject(hRgn2);

    } while (FindNextFileA(hFind, &find));

    FindClose(hFind);
    return 0;
}
