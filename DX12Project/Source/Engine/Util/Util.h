#pragma once
#include <windows.h>
#include <string>

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifndef SafeDelete
#define SafeDelete(x) { if (x) { delete(x); x = nullptr; } }
#endif

#ifndef SafeDeleteArray
#define SafeDeleteArray(x) { delete[](x); *x = nullptr; }
#endif

#ifndef SafeDeleteVector
#define SafeDeleteVector(vec) { \
		for (auto it : vec) \
		{ \
			SafeDelete(it); \
		} \
		vec.clear(); \
	}
#endif

const int INVALID_INDEX = -1;

class Uncopyable
{
protected:
	Uncopyable() {}
	~Uncopyable() {}

private:
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};