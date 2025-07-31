#pragma once

class CFont
{
public:
    HFONT m_hFont;

    operator HFONT()
    {
        return m_hFont;
    }

    CFont(HFONT hFont = NULL) : m_hFont(hFont)
    {
    }

    CFont& operator=(HFONT hFont)
    {
        if (m_hFont)
            ::DeleteObject(m_hFont);
        m_hFont = hFont;
        return *this;
    }

    ~CFont()
    {
        if (m_hFont)
            ::DeleteObject(m_hFont);
    }

private:
    CFont(const CFont& font);
};

class CBitmap
{
public:
    HBITMAP m_hBitmap;

    operator HBITMAP()
    {
        return m_hBitmap;
    }

    CBitmap(HBITMAP hBitmap = NULL) : m_hBitmap(hBitmap)
    {
    }

    CBitmap& operator=(HBITMAP hBitmap)
    {
        if (m_hBitmap)
            ::DeleteObject(m_hBitmap);
        m_hBitmap = hBitmap;
        return *this;
    }

    ~CBitmap()
    {
        if (m_hBitmap)
            ::DeleteObject(m_hBitmap);
    }

    void Swap(CBitmap& other)
    {
        HBITMAP hbmTmp = m_hBitmap;
        m_hBitmap = other.m_hBitmap;
        other.m_hBitmap = hbmTmp;
    }

private:
    CBitmap(const CBitmap& bitmap);
};

class CRgn
{
public:
    HRGN m_hRgn;

    operator HRGN()
    {
        return m_hRgn;
    }

    CRgn() : m_hRgn(::CreateRectRgn(0, 0, 0, 0))
    {
    }

    CRgn(HRGN hRgn) : m_hRgn(hRgn)
    {
    }

    CRgn(const CRgn& rgn) : m_hRgn(::CreateRectRgn(0, 0, 0, 0))
    {
        ::CombineRgn(m_hRgn, rgn.m_hRgn, NULL, RGN_COPY);
    }

    CRgn& operator=(HRGN hRgn)
    {
        ::CombineRgn(m_hRgn, hRgn, NULL, RGN_COPY);
        return *this;
    }

    ~CRgn()
    {
        if (m_hRgn)
            ::DeleteObject(m_hRgn);
    }
};

class CDC
{
public:
    HDC m_hDC;
    HWND m_hWnd;

    operator HDC()
    {
        return m_hDC;
    }

    CDC(HDC hdcRef = NULL) : m_hDC(::CreateCompatibleDC(hdcRef)), m_hWnd(NULL)
    {
    }

    CDC(HWND hwnd) : m_hDC(::GetDC(hwnd)), m_hWnd(hwnd)
    {
    }

    CDC(const CDC& dcRef) : m_hDC(::CreateCompatibleDC(dcRef.m_hDC)), m_hWnd(NULL)
    {
    }

    ~CDC()
    {
        if (m_hWnd)
            ::ReleaseDC(m_hWnd, m_hDC);
        else
            ::DeleteDC(m_hDC);
    }

private:
    CRgn& operator=(HRGN hRgn);
};
