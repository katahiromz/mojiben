#pragma once

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

    ~CDC()
    {
        if (m_hWnd)
            ::ReleaseDC(m_hWnd, m_hDC);
        else
            ::DeleteDC(m_hDC);
    }
};
