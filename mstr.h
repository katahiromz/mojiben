// mstr.h - string manipulation
#pragma once

#include <string>
#include <cctype>

template <typename T_CHAR>
inline void mstr_trim(std::basic_string<T_CHAR>& str, const T_CHAR *spaces) {
    typedef std::basic_string<T_CHAR> string_type;
    size_t i = str.find_first_not_of(spaces);
    size_t j = str.find_last_not_of(spaces);
    if ((i == string_type::npos) || (j == string_type::npos)) {
        str.clear();
    } else {
        str = str.substr(i, j - i + 1);
    }
}

template <typename T_CHAR>
inline void mstr_trim_left(std::basic_string<T_CHAR>& str, const T_CHAR *spaces) {
    typedef std::basic_string<T_CHAR> string_type;
    size_t i = str.find_first_not_of(spaces);
    if (i == string_type::npos) {
        str.clear();
    } else {
        str = str.substr(i);
    }
}

template <typename T_CHAR>
inline void mstr_trim_right(std::basic_string<T_CHAR>& str, const T_CHAR *spaces) {
    typedef std::basic_string<T_CHAR> string_type;
    size_t j = str.find_last_not_of(spaces);
    if (j == string_type::npos) {
        str.clear();
    } else {
        str = str.substr(0, j + 1);
    }
}

template <typename T_STR>
inline bool
mstr_replace(T_STR& str, const T_STR& from, const T_STR& to) {
    bool ret = false;
    size_t i = 0;
    for (;;) {
        i = str.find(from, i);
        if (i == T_STR::npos)
            break;
        ret = true;
        str.replace(i, from.size(), to);
        i += to.size();
    }
    return ret;
}

template <typename T_STR>
inline bool
mstr_replace(T_STR& str, const typename T_STR::value_type *from, const typename T_STR::value_type *to) {
    return mstr_replace(str, T_STR(from), T_STR(to));
}

template <typename T_STR_CONTAINER>
inline void
mstr_split(T_STR_CONTAINER& container,
           const typename T_STR_CONTAINER::value_type& str,
           const typename T_STR_CONTAINER::value_type& chars)
{
    container.clear();
    size_t i = 0, k = str.find_first_of(chars);
    while (k != T_STR_CONTAINER::value_type::npos) {
        container.push_back(str.substr(i, k - i));
        i = k + 1;
        k = str.find_first_of(chars, i);
    }
    container.push_back(str.substr(i));
}

template <typename T_STR_CONTAINER>
inline typename T_STR_CONTAINER::value_type
mstr_join(const T_STR_CONTAINER& container,
          const typename T_STR_CONTAINER::value_type& sep)
{
    typename T_STR_CONTAINER::value_type result;
    typename T_STR_CONTAINER::const_iterator it, end;
    it = container.begin();
    end = container.end();
    if (it != end)
    {
        result = *it;
        for (++it; it != end; ++it)
        {
            result += sep;
            result += *it;
        }
    }
    return result;
}

template <typename T_STR_CONTAINER>
inline typename T_STR_CONTAINER::value_type
mstr_join(const T_STR_CONTAINER& container,
          const typename T_STR_CONTAINER::value_type::value_type *sep)
{
    return mstr_join(container, T_STR_CONTAINER::value_type(sep));
}

static std::wstring mstr_unescape(const std::wstring& str) {
    std::wstring ret;
    ret.reserve(str.size());

    for (size_t ich = 0; ich < str.size(); ++ich) {
        if (str[ich] == L'\\' && ich + 1 < str.size()) {
            ++ich;
            switch (str[ich]) {
            case L'\'': case L'\"': case L'?': case L'\\':
                ret += str[ich];
                break;

            case L'a': ret += L'\a'; break;
            case L'b': ret += L'\b'; break;
            case L'f': ret += L'\f'; break;
            case L'n': ret += L'\n'; break;
            case L'r': ret += L'\r'; break;
            case L't': ret += L'\t'; break;
            case L'v': ret += L'\v'; break;

            case L'x': { // hex (\xNN)
                size_t start = ich + 1, len = 0;
                while (start + len < str.size() && iswxdigit(str[start + len]))
                    ++len;
                if (len > 0) {
                    int value = std::wcstol(str.substr(start, len).c_str(), NULL, 16);
                    ret += static_cast<wchar_t>(value);
                    ich += len;
                }
                break;
            }

            case L'u': { // Unicode (\uNNNN)
                if (ich + 4 < str.size()) {
                    int value = std::wcstol(str.substr(ich + 1, 4).c_str(), NULL, 16);
                    ret += static_cast<wchar_t>(value);
                    ich += 4;
                }
                break;
            }

            default:
                ret += L'\\';
                ret += str[ich];
                break;
            }
        } else {
            ret += str[ich];
        }
    }

    return ret;
}

inline
std::wstring mstr_unquote(const std::wstring& str) {
    if (str.size() >= 2 && str[0] == L'"') {
        if (str[str.size() - 1] == L'"')
            return mstr_unescape(str.substr(1, str.size() - 2));
        return mstr_unescape(str.substr(1, str.size() - 1));
    }
    return str;
}

#include <windows.h>

inline bool mstr_is_utf8(const std::string& binary) {
    if (binary.empty())
        return true;
    return !!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, binary.c_str(), (INT)binary.size(), NULL, 0);
}

inline std::wstring mstr_ansi_to_wide(UINT codepage, const std::string& ansi) {
    std::wstring wide;
    if (ansi.empty())
        return wide;
    INT len = MultiByteToWideChar(codepage, 0, ansi.c_str(), (INT)ansi.size(), NULL, 0);
    if (!len)
        return wide;
    wide.resize(len);
    MultiByteToWideChar(codepage, 0, ansi.c_str(), (INT)ansi.size(), &wide[0], len);
    return wide;
}

inline std::string mstr_wide_to_ansi(UINT codepage, const std::wstring& wide) {
    std::string ansi;
    if (ansi.empty())
        return ansi;
    INT len = WideCharToMultiByte(codepage, 0, wide.c_str(), (INT)wide.size(), NULL, 0, NULL, NULL);
    if (!len)
        return ansi;
    ansi.resize(len);
    WideCharToMultiByte(codepage, 0, wide.c_str(), (INT)wide.size(), &ansi[0], len, NULL, NULL);
    return ansi;
}
