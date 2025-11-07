// MyLib.cpp --- The media library
// License: MIT
#include <windows.h>
#include <olectl.h>
#include <shlwapi.h>
#include <cstdlib>
#include <cstring>
#include <process.h>
#include <wchar.h>
#include <tchar.h>
#include <assert.h>
#include "MyLib.h"
#include "read_all.h"
#include "../mstr.h"

class AutoPriority {
public:
    AutoPriority() {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    }
    ~AutoPriority() {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    }
};

////////////////////////////////////////////////////////////////////////////

std::wstring MyLibStringTable::key_at(size_t i) {
    if (i >= size())
        return L"";
    return m_pairs[i].m_key;
}

std::wstring MyLibStringTable::value_at(size_t i) {
    if (i >= size())
        return L"";
    return m_pairs[i].m_value;
}

std::wstring MyLibStringTable::operator[](const std::wstring& key) {
    for (size_t i = 0; i < m_pairs.size(); ++i) {
        if (m_pairs[i].m_key == key)
            return m_pairs[i].m_value;
    }
    return L"";
}

std::wstring MyLibStringTable::operator[](int key) {
    wchar_t str[64];
    wsprintfW(str, L"%04u", (WORD)key);
    return (*this)[str];
}

void MyLibStringTable::set_text(std::wstring& text) {
    mstr_replace(text, L"\r\n", L"\n");
    mstr_trim_right(text, L" \t\r\n　");

    std::vector<std::wstring> lines;
    mstr_split(lines, text, L"\n");

    const std::wstring sep = L"=";

    for (size_t iLine = 0; iLine < lines.size(); ++iLine) {
        std::wstring& line = lines[iLine];
        mstr_trim(line, L" \t\r\n　");
        if (line.empty() || line[0] == L'#')
            continue;

        size_t ich = line.find(sep);
        if (ich == line.npos) {
            MyLibStringPair pair = { line, L"" };
            m_pairs.push_back(pair);
            continue;
        }

        std::wstring key = line.substr(0, ich);
        std::wstring value = line.substr(ich + sep.size());
        mstr_trim(key, L" \t\r\n　");
        mstr_trim(value, L" \t\r\n　");
        MyLibStringPair pair = { key, value };
        m_pairs.push_back(pair);
    }
}

////////////////////////////////////////////////////////////////////////////

static HRESULT s_hrOleInit;
MyLib *MyLib::s_pThis = NULL;

MyLib::MyLib() {
    s_pThis = this;
    s_hrOleInit = ::OleInitialize(NULL);
}

MyLib::~MyLib() {
    if (SUCCEEDED(s_hrOleInit))
        ::OleUninitialize();
}

std::wstring MyLib::_find_data_dir() {
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, _countof(path));
    PathRemoveFileSpecW(path);

    PathAppendW(path, L"data");
    if (PathIsDirectoryW(path))
        return path;
    PathRemoveFileSpecW(path);

    PathRemoveFileSpecW(path);
    PathAppendW(path, L"data");
    if (PathIsDirectoryW(path))
        return path;
    PathRemoveFileSpecW(path);

    PathRemoveFileSpecW(path);
    PathAppendW(path, L"data");
    if (PathIsDirectoryW(path))
        return path;

    return L"data";
}

std::wstring MyLib::find_data_dir() {
    if (m_data_dir.empty())
        m_data_dir = _find_data_dir();
    return m_data_dir;
}

std::wstring MyLib::find_data_file(const std::wstring& filename) {
    if (!PathIsRelativeW(filename.c_str()))
        return filename;

    std::wstring path = find_data_dir();
    path += L"\\";
    path += filename;

    return path;
}

bool MyLib::load_binary(std::string& binary, const std::wstring& filename) {
    std::wstring path = find_data_file(filename);

    if (!read_all(binary, path.c_str())) {
        assert(0);
        return false;
    }

    return true;
}

bool MyLib::save_binary(const std::string& binary, const std::wstring& filename) {
    std::wstring path = find_data_file(filename);

    FILE *fout = _tfopen(path.c_str(), L"wb");
    if (!fout) {
        assert(0);
        return false;
    }

    if (!fwrite(binary.c_str(), binary.size(), 1, fout)) {
        assert(0);
        fclose(fout);
        return false;
    }

    fclose(fout);
    return true;
}

bool MyLib::load_utf8_text_file(std::string& binary, const std::wstring& filename) {
    if (!load_binary(binary, filename))
        return false;

    if (binary.size() >= 3 && std::memcmp(binary.c_str(), "\xEF\xBB\xBF", 3) == 0) { // UTF-8 BOM
        binary.erase(0, 3);
    } else if (binary.size() >= 2) {
        if (std::memcmp(binary.c_str(), "\xFF\xFE", 2) == 0 || std::memcmp(binary.c_str(), "\xFE\xFF", 2) == 0) {
            // UTF-16 は未対応
            return false;
        }
    }

    return true;
}

bool MyLib::play_sound(const std::wstring& filename) {
    int err;
    {
        AutoPriority high_priority;
        TCHAR szCommand[MAX_PATH + 64];
        wsprintf(szCommand, TEXT("open \"%s\" type mpegvideo alias myaudio"), filename.c_str());
        err = mciSendString(szCommand, NULL, 0, 0);
        if (err) {
            assert(0);
            return false;
        }
    }
    err = mciSendString(TEXT("play myaudio wait"), NULL, 0, 0);
    assert(!err);
    err = mciSendString(TEXT("close myaudio"), NULL, 0, 0);
    assert(!err);
    return true;
}

unsigned __stdcall MyLib::_play_sound_async_proc(void *arg) {
    wchar_t *filename = (wchar_t *)arg;
    s_pThis->play_sound(filename);
    std::free(filename);
    return 0;
}

bool MyLib::play_sound_async(const std::wstring& filename) {
    wchar_t *file_ref = _wcsdup(filename.c_str());
    if (!file_ref) {
        assert(0);
        return false;
    }

    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, MyLib::_play_sound_async_proc, file_ref, 0, NULL);
    CloseHandle(hThread);
    return hThread != NULL;
}

// NOTE: OleInitialize / OleUninitialize が必要。
HBITMAP MyLib::load_picture(const std::wstring& filename) {
    std::string binary;
    if (!load_binary(binary, filename)) {
        return NULL;
    }

    if (binary.empty()) {
        assert(0);
        return NULL;
    }

    IStream *pStream = SHCreateMemStream((PBYTE)&binary[0], (DWORD)binary.size());
    if (!pStream) {
        return NULL;
    }

    IPicture *pPicture;
    HRESULT hr = OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (void**)&pPicture);
    if (FAILED(hr)) {
        pStream->Release();
        return NULL;
    }

    OLE_HANDLE hPic = NULL;
    pPicture->get_Handle(&hPic);
    HBITMAP hBitmap = (HBITMAP)CopyImage((HBITMAP)hPic, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    pPicture->Release();
    pStream->Release();
    return hBitmap;
}

bool MyLib::load_utf8_text_file_as_wide(std::wstring& text, const std::wstring& filename) {
    text.clear();

    std::string binary;
    if (!load_utf8_text_file(binary, filename)) {
        return false;
    }

    text = mstr_ansi_to_wide(CP_UTF8, binary);
    return true;
}

bool MyLib::load_string_table(MyLibStringTable& table, const std::wstring& filename) {
    table.clear();

    std::wstring wide;
    if (!load_utf8_text_file_as_wide(wide, filename)) {
        return false;
    }

    table.set_text(wide);
    return true;
}
