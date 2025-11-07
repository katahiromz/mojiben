// read_all.cpp
#include "read_all.h"
#include <new>

// TODO: Large file
bool read_all(std::string& binary, LPCTSTR file_name) {
    binary.clear();

    // Open a file
    HANDLE hFile;
    DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD flags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;
    hFile = ::CreateFile(file_name, GENERIC_READ, share_mode, NULL, OPEN_EXISTING, flags, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // Get file size
    DWORD high;
    DWORD low = ::GetFileSize(hFile, &high);
    if (low == INVALID_FILE_SIZE) {
        DWORD error = ::GetLastError();
        if (error) {
            ::CloseHandle(hFile);
            ::SetLastError(error);
            return false;
        }
    }
    size_t size = low;
    if (high) {
        ::CloseHandle(hFile);
        ::SetLastError(ERROR_FILE_TOO_LARGE);
        return false;
    }

    if (!size) { // empty file
        ::CloseHandle(hFile);
        return true;
    }

    // Allocate buffer
    try {
        binary.resize(size);
    } catch (const std::bad_alloc&) {
        ::CloseHandle(hFile);
        ::SetLastError(ERROR_OUTOFMEMORY);
        return false;
    }

    // Read
    DWORD dwHadRead;
    if (!::ReadFile(hFile, &binary[0], (DWORD)size, &dwHadRead, NULL)) {
        DWORD error = ::GetLastError();
        ::CloseHandle(hFile);
        ::SetLastError(error);
        return false;
    }
    if (dwHadRead < binary.size())
        binary.resize(dwHadRead);

    // Done
    ::CloseHandle(hFile);
    return true;
}
