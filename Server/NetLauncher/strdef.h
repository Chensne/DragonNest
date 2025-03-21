
#pragma once

#include <tchar.h>
#include <string>
#include <ShellAPI.h>

#pragma comment(lib, "shell32.lib")

#ifndef UNICODE
#define std_str std::string
#else
#define std_str std::wstring
#endif
