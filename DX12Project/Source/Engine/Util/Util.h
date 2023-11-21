#pragma once
#include <windows.h>
#include <string>

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

namespace Util
{
	const std::wstring ShaderPath = L"./Shaders/ByteCodes/";
	const std::wstring AssetPath = L"../Resources/";

	inline std::wstring AnsiToWString(const std::string& InStr)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, InStr.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	inline std::wstring GetShaderAssetFullPath(const std::wstring& InAssetName)
	{
		return ShaderPath + InAssetName;
	}

	inline std::wstring GetAssetFullPath(const std::wstring& InAssetName)
	{
		return AssetPath + InAssetName;
	}
};