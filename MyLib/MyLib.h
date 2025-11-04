// MyLib.h --- The media library
// License: MIT
#pragma once

#include <string>
#include <vector>

struct MyLibStringPair {
    std::wstring m_key, m_value;
};

class MyLibStringTable {
public:
    void set_text(std::wstring& text);
    std::wstring operator[](const wchar_t *key);
    std::wstring operator[](int key);

    std::vector<MyLibStringPair> m_pairs;
    void clear() { m_pairs.clear(); }
};

class MyLib {
public:
    MyLib();
    virtual ~MyLib();

    std::wstring find_data_dir();
    std::wstring find_data_file(const wchar_t *filename, const wchar_t *section = NULL);

    bool load_binary(std::string& binary, const wchar_t *filename, const wchar_t *section = NULL);
    bool save_binary(const std::string& binary, const wchar_t *filename, const wchar_t *section = NULL);

    bool load_utf8_text_file(std::string& binary, const wchar_t *filename, const wchar_t *section = NULL);
    bool load_utf8_text_file_as_wide(std::wstring& text, const wchar_t *filename, const wchar_t *section = NULL);
    HBITMAP load_picture(const wchar_t *filename, const wchar_t *section = NULL);
    bool load_string_table(MyLibStringTable& table, const wchar_t *filename, const wchar_t *section = NULL);

    bool save_temp_file(std::wstring& path, const std::string& binary);

    static bool play_sound(const wchar_t *temp_file);
    static bool play_sound_async(const wchar_t *temp_file);

protected:
    std::wstring m_data_dir;

    std::wstring _find_data_dir();
    static unsigned __stdcall MyLib::_play_sound_async_proc(void *arg);
};
