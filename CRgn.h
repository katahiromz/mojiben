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
