#pragma once

#include <windows.h>
#include <string>

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ReturnCheckAssert
#define ReturnCheckAssert(x) { assert(x); return x; }
#endif

#ifndef SafeDelete
#define SafeDelete(x) { delete(x); x = nullptr; }
#endif