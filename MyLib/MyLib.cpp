// MyLib.cpp --- The media library
// License: MIT
#include <windows.h>
#include <olectl.h>
#include <shlwapi.h>
#include <cstdlib>
#include <cstring>
#include <tchar.h>
#include <assert.h>
#include "MyLib.h"
#include "read_all.h"
#include "mstr.h"

class AutoPriority
{
public:
    AutoPriority()
    {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    }
    ~AutoPriority()
    {
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    }
};

////////////////////////////////////////////////////////////////////////////

std::wstring MyLibStringTable::operator[](const wchar_t *key) {
    for (size_t i = 0; i < m_pairs.size(); ++i) {
        if (m_pairs[i].m_key == key)
            return m_pairs[i].m_value;
    }
    return L"";
}

std::wstring MyLibStringTable::operator[](int key) {
    wchar_t str[64];
    wsprintfW(str, L"%08u", key);
    return (*this)[str];
}

void MyLibStringTable::set_text(std::wstring& text) {
    mstr_replace(text, L"\r\n", L"\n");

    std::vector<std::wstring> lines;
    mstr_split(lines, text, L"\n");

    const std::wstring sep = L">>";

    for (size_t iLine = 0; iLine < lines.size(); ++iLine) {
        std::wstring& line = lines[iLine];
        mstr_trim(line, L" \t\r\n");

        size_t ich = line.find(sep);
        if (ich == line.npos) {
            MyLibStringPair pair = { line, L"" };
            m_pairs.push_back(pair);
            continue;
        }

        std::wstring key = line.substr(0, ich);
        std::wstring value = line.substr(ich + sep.size());
        mstr_trim(key, L" \t\r\n");
        mstr_trim(value, L" \t\r\n");
        MyLibStringPair pair = { key, value };
        m_pairs.push_back(pair);
    }
}

////////////////////////////////////////////////////////////////////////////

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

std::wstring MyLib::find_data_file(const wchar_t *filename, const wchar_t *section) {
    std::wstring path = find_data_dir();

    if (section) {
        path += L"\\";
        path += section;
    }

    path += L"\\";
    path += filename;

    if (!PathFileExistsW(path.c_str()))
        return path;

    assert(0);
    return filename;
}

bool MyLib::load_binary(std::string& binary, const wchar_t *filename, const wchar_t *section) {
    std::wstring path = find_data_file(filename, section);

    if (!read_all(binary, path.c_str())) {
        assert(0);
        return false;
    }

    return true;
}

bool MyLib::load_utf8_text_file(std::string& binary, const wchar_t *filename, const wchar_t *section) {
    if (!load_binary(binary, filename, section))
        return false;

    if (binary.size() >= 3 && std::memcmp(binary.c_str(), "\xEF\xBB\xBF", 3) == 0) { // UTF-8 BOM
        binary.erase(0, 3);
    }

    return true;
}

bool MyLib::save_temp_file(std::wstring& temp_file, const std::string& binary) {
    temp_file.clear();

    TCHAR szTempPath[MAX_PATH];
    GetTempPath(_countof(szTempPath), szTempPath);

    TCHAR szFile[MAX_PATH];
    GetTempFileName(szTempPath, TEXT("MJB"), 0, szFile);

    FILE *fout = _tfopen(szFile, TEXT("wb"));
    if (!fout)
        return false;
    if (!binary.empty())
        fwrite(&binary[0], binary.size(), 1, fout);
    fclose(fout);

    temp_file = szFile;
    return true;
}

bool MyLib::play_sound(const wchar_t *temp_file) {
    {
        AutoPriority high_priority;
        TCHAR szCommand[MAX_PATH + 64];
        wsprintf(szCommand, TEXT("open \"%s\" type mpegvideo alias myaudio"), temp_file);
        mciSendString(szCommand, NULL, 0, 0);
    }
    mciSendString(TEXT("play myaudio wait"), NULL, 0, 0);
    mciSendString(TEXT("close myaudio"), NULL, 0, 0);
    DeleteFile(temp_file);
    return true;
}

DWORD WINAPI MyLib::_play_sound_async_proc(LPVOID arg) {
    wchar_t *temp_file = (wchar_t *)arg;
    MyLib::play_sound(temp_file);
    return 0;
}

bool MyLib::play_sound_async(const wchar_t *temp_file) {
    HANDLE hThread = CreateThread(NULL, 0, MyLib::_play_sound_async_proc, (void *)temp_file, 0, NULL);
    CloseHandle(hThread);
    return hThread != NULL;
}

HBITMAP MyLib::load_picture(const wchar_t *filename, const wchar_t *section) {
    std::string binary;
    if (!load_binary(binary, filename, section)) {
        assert(0);
        return NULL;
    }

    IStream *pStream = SHCreateMemStream((PBYTE)&binary[0], (DWORD)binary.size());
    if (!pStream) {
        assert(0);
        return NULL;
    }

    IPicture *pPicture;
    HRESULT hr = OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (void**)&pPicture);
    if (FAILED(hr)) {
        assert(0);
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

bool MyLib::load_utf8_text_file_as_wide(std::wstring& text, const wchar_t *filename, const wchar_t *section) {
    text.clear();

    std::string binary;
    if (!load_utf8_text_file(binary, filename, section)) {
        assert(0);
        return false;
    }

    text = mstr_ansi_to_wide(CP_UTF8, binary);
    return true;
}

bool MyLib::load_string_table(MyLibStringTable& table, const wchar_t *filename, const wchar_t *section) {
    table.clear();

    std::wstring wide;
    if (!load_utf8_text_file_as_wide(wide, filename, section)) {
        return false;
    }

    table.set_text(wide);
    return true;
}
