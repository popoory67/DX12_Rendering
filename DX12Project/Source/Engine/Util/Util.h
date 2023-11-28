#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

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

	std::wstring AnsiToWString(const std::string& InStr);
	std::wstring GetShaderAssetFullPath(const std::wstring& InAssetName);
	std::wstring GetAssetFullPath(const std::wstring& InAssetName);

	std::vector<char> GetBinaryData(const std::wstring& InPath);
};