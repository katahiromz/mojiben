class KanjiDataDlg
{
public:
    std::wstring m_text;
    std::wstring m_reading;
    std::wstring m_meaning;
    std::wstring m_examples;

    KanjiDataDlg() : m_hwnd(NULL) { }
    virtual ~KanjiDataDlg() { }

    static KanjiDataDlg *get_self(HWND hwnd) {
        return (KanjiDataDlg *)GetWindowLongPtrW(hwnd, DWLP_USER);
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        center_window(hwnd);

        LOGFONT lf;
        ZeroMemory(&lf, sizeof(lf));
        lstrcpyn(lf.lfFaceName, TEXT("ピザPゴシック"), _countof(lf.lfFaceName));
        lf.lfHeight = -26;
        lf.lfWeight = FW_NORMAL;
        lf.lfCharSet = SHIFTJIS_CHARSET;
        lf.lfQuality = PROOF_QUALITY;
        m_hNormalFont = CreateFontIndirect(&lf);

        lf.lfHeight = -118;
        m_hBigFont = CreateFontIndirect(&lf);

        SetDlgItemTextW(hwnd, edt1, m_text.c_str());
        SetDlgItemTextW(hwnd, edt2, m_reading.c_str());
        SetDlgItemTextW(hwnd, edt3, m_meaning.c_str());
        SetDlgItemTextW(hwnd, edt4, m_examples.c_str());

        SendDlgItemMessageW(hwnd, edt3, FC_SETRUBYRATIO, 4, 6);
        SendDlgItemMessageW(hwnd, edt4, FC_SETRUBYRATIO, 4, 6);

        SendDlgItemMessageW(hwnd, edt1, WM_SETFONT, (WPARAM)m_hBigFont, TRUE);
        SendDlgItemMessageW(hwnd, edt2, WM_SETFONT, (WPARAM)m_hNormalFont, TRUE);
        SendDlgItemMessageW(hwnd, edt3, WM_SETFONT, (WPARAM)m_hNormalFont, TRUE);
        SendDlgItemMessageW(hwnd, edt4, WM_SETFONT, (WPARAM)m_hNormalFont, TRUE);

        SetFocus(GetDlgItem(hwnd, IDOK));
        return FALSE;
    }

    void OnDestroy(HWND hwnd)
    {
        DeleteObject(m_hNormalFont);
        DeleteObject(m_hBigFont);
        SetWindowLongPtrW(hwnd, DWLP_USER, 0);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id) {
        case IDOK:
        case IDCANCEL:
            ::EndDialog(hwnd, id);
            break;
        }
    }

    HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
    {
        if (type == CTLCOLOR_STATIC) {
            WCHAR cls[64];
            ::GetClassNameW(hwndChild, cls, _countof(cls));
            if (lstrcmpiW(cls, L"EDIT") == 0 ||
                lstrcmpiW(cls, L"FuriganaCtl") == 0)
            {
                return GetSysColorBrush(COLOR_WINDOW);
            }
        }
        return NULL;
    }

    static INT_PTR CALLBACK
    DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        KanjiDataDlg *self = get_self(hwnd);
        if (!self) {
            if (uMsg == WM_INITDIALOG) {
                self = (KanjiDataDlg *)lParam;
                self->m_hwnd = hwnd;
                SetWindowLongPtrW(hwnd, DWLP_USER, (LONG_PTR)self);
            } else {
                return 0;
            }
        }
        switch (uMsg) {
        HANDLE_MSG(hwnd, WM_INITDIALOG, self->OnInitDialog);
        HANDLE_MSG(hwnd, WM_DESTROY, self->OnDestroy);
        HANDLE_MSG(hwnd, WM_COMMAND, self->OnCommand);
        HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC, self->OnCtlColor);
        default: break;
        }
        return 0;
    }

    INT_PTR dialog_box(HINSTANCE hInst, HWND hwndParent) {
        return DialogBoxParamW(hInst, MAKEINTRESOURCEW(100), hwndParent, DialogProc, (LPARAM)this);
    }

protected:
    HWND m_hwnd;
    HFONT m_hNormalFont;
    HFONT m_hBigFont;
};
