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
    std::wstring operator[](const std::wstring& key);
    std::wstring operator[](int key);

    std::vector<MyLibStringPair> m_pairs;
    size_t size() const { return m_pairs.size(); }
    void clear() { m_pairs.clear(); }
    std::wstring key_at(size_t i);
    std::wstring value_at(size_t i);
};

class MyLib {
public:
    MyLib();
    virtual ~MyLib();

    std::wstring find_data_dir();
    std::wstring find_data_file(const std::wstring& filename);

    bool load_binary(std::string& binary, const std::wstring& filename);
    bool save_binary(const std::string& binary, const std::wstring& filename);

    bool load_utf8_text_file(std::string& binary, const std::wstring& filename);
    bool load_utf8_text_file_as_wide(std::wstring& text, const std::wstring& filename);
    HBITMAP load_picture(const std::wstring& filename);
    bool load_string_table(MyLibStringTable& table, const std::wstring& filename);

    bool play_sound(const std::wstring& filename);
    bool play_sound_async(const std::wstring& filename);

protected:
    std::wstring m_data_dir;
    static MyLib *s_pThis;

    std::wstring _find_data_dir();
    bool _save_temp_file(std::wstring& path, const std::string& binary);
    static unsigned __stdcall MyLib::_play_sound_async_proc(void *arg);
};
