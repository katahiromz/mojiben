// read_all.h
#pragma once

#include <string>
#ifndef _INC_WINDOWS
    #include <windows.h>
#endif

bool read_all(std::string& binary, LPCTSTR file_name);
