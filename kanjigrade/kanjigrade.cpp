// kanjigrade.cpp --- 漢字の習う学年を調べるプログラム。
// Author: katahiromz
// License: MIT
#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include <string>
#include "../mstr.h"
#include "kanjigrade.h"

void version() {
    puts("0.0");
}

void usage() {
    _putws(
        L"使用例: kanjigrade [オプション <漢字>\n"
        L"\n"
        L"オプション:\n"
        L" -v, --verbose    詳細表示。\n"
        L" --help           このメッセージを表示。\n"
        L" --version        バージョン表示。"
    );
}

// 補助関数
inline bool is_char_hiragana(wchar_t ch) {
    return ((L'ぁ' <= ch && ch <= L'ん') || ch == L'ー' || ch == L'ゔ');
}
inline bool is_char_katakana(wchar_t ch) {
    return ((L'ァ' <= ch && ch <= L'ン') || ch == L'ー' || ch == L'ヴ');
}
inline bool is_char_kana(wchar_t ch) {
    return is_char_hiragana(ch) || is_char_katakana(ch);
}
inline bool is_char_digit(wchar_t ch) {
    return ((L'0' <= ch && ch <= L'9') || (L'０' <= ch && ch <= L'９'));
}
inline bool is_char_lower(wchar_t ch) {
    return ((L'a' <= ch && ch <= L'z') || (L'ａ' <= ch && ch <= L'ｚ'));
}
inline bool is_char_upper(wchar_t ch) {
    return ((L'A' <= ch && ch <= L'Z') || (L'Ａ' <= ch && ch <= L'Ｚ'));
}
inline bool is_char_alpha(wchar_t ch) {
    return is_char_lower(ch) || is_char_upper(ch);
}
inline bool is_char_alpha_numeric(wchar_t ch) {
    return is_char_alpha(ch) || is_char_digit(ch);
}
inline bool is_char_kanji(wchar_t ch) {
    return ((0x3400 <= ch && ch <= 0x9FFF) || (0xF900 <= ch && ch <= 0xFAFF) || ch == 0x3005 || ch == 0x3007);
}

int kanjigrade(std::wstring& str, bool verbose) {
    // 文字列の前後の空白を取り除く
    mstr_trim(str, L" \t\r\n　");

    // 漢字と学年
    std::wstring kanji_grade_1 = KANJI_GRADE_1;
    std::wstring kanji_grade_2 = KANJI_GRADE_2;
    std::wstring kanji_grade_3 = KANJI_GRADE_3;
    std::wstring kanji_grade_4 = KANJI_GRADE_4;
    std::wstring kanji_grade_5 = KANJI_GRADE_5;
    std::wstring kanji_grade_6 = KANJI_GRADE_6;

    int grade = 0;
    for (size_t ich = 0; ich < str.size(); ++ich) {
        wchar_t wch = str[ich];
        if (is_char_kana(wch)) {
            static bool check = false;
            if (!check) {
                check = true;
                if (verbose) _putws(L"カナを習うのは、小学一年生から。");
            }
            if (grade < 1) grade = 1;
            continue;
        }
        if (is_char_alpha(wch)) {
            static bool check = false;
            if (!check) {
                check = true;
                if (verbose) _putws(L"英語を習うのは、小学三年生から。");
            }
            if (grade < 3) grade = 3;
            continue;
        }
        if (is_char_digit(wch)) {
            static bool check = false;
            if (!check) {
                check = true;
                if (verbose) _putws(L"数字を習うのは、小学一年生から。");
            }
            if (grade < 1) grade = 1;
            continue;
        }
        if (wch == L'々') {
            static bool check = false;
            if (!check) {
                check = true;
                if (verbose) _putws(L"'々'を習うのは、小学三年生から。");
            }
            if (grade < 3) grade = 3;
            continue;
        }
        if (!is_char_kanji(wch)) {
            continue;
        }
        // 漢字の学年を検査する
        if (kanji_grade_1.find(wch) != kanji_grade_1.npos) {
            if (verbose) wprintf(L"「%lc」 は小学一年生の漢字です。\n", wch);
            if (grade < 1) grade = 1;
            continue;
        }
        if (kanji_grade_2.find(wch) != kanji_grade_2.npos) {
            if (verbose) wprintf(L"「%lc」 は小学二年生の漢字です。\n", wch);
            if (grade < 2) grade = 2;
            continue;
        }
        if (kanji_grade_3.find(wch) != kanji_grade_3.npos) {
            if (verbose) wprintf(L"「%lc」 は小学三年生の漢字です。\n", wch);
            if (grade < 3) grade = 3;
            continue;
        }
        if (kanji_grade_4.find(wch) != kanji_grade_4.npos) {
            if (verbose) wprintf(L"「%lc」 は小学四年生の漢字です。\n", wch);
            if (grade < 4) grade = 4;
            continue;
        }
        if (kanji_grade_5.find(wch) != kanji_grade_5.npos) {
            if (verbose) wprintf(L"「%lc」 は小学五年生の漢字です。\n", wch);
            if (grade < 5) grade = 5;
            continue;
        }
        if (kanji_grade_6.find(wch) != kanji_grade_6.npos) {
            if (verbose) wprintf(L"「%lc」 は小学六年生の漢字です。\n", wch);
            if (grade < 6) grade = 6;
            continue;
        }
        if (verbose) wprintf(L"「%lc」 は中学生以上の漢字です。\n", wch);
        if (grade < 7) grade = 7; // 中学生以上
    }

    // 結果を取得
    const wchar_t *result;
    switch (grade) {
    case 0: result = L"Grade 0／幼稚園児"; break;
    case 1: result = L"Grade 1／小学一年生"; break;
    case 2: result = L"Grade 2／小学二年生"; break;
    case 3: result = L"Grade 3／小学三年生"; break;
    case 4: result = L"Grade 4／小学四年生"; break;
    case 5: result = L"Grade 5／小学五年生"; break;
    case 6: result = L"Grade 6／小学六年生"; break;
    default: result = L"Grade 7+／中学生以上"; break;
    }

    // 結果を出力
    wprintf(L"\nResult: %ls\n", result);
    fflush(stdout);
    return grade;
}

int _wmain(int argc, wchar_t **wargv) {
    setlocale(LC_CTYPE, "");

    if (argc <= 1) {
        usage();
        return 999;
    }

    std::wstring str;
    bool verbose = false;
    for (int iarg = 1; iarg < argc; ++iarg) {
        std::wstring arg = wargv[iarg];
        if (arg == L"--help") {
            usage();
            return 999;
        }
        if (arg == L"--version") {
            version();
            return 999;
        }
        if (arg == L"--verbose" || arg == L"-v") {
            verbose = true;
            continue;
        }
        str += arg;
    }

    return kanjigrade(str, verbose);
}

int main(void) {
    INT argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    INT ret = _wmain(argc, wargv);
    LocalFree(wargv);
    return ret;
}
