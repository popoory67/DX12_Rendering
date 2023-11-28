#include "Util.h"

namespace Util
{
    std::wstring AnsiToWString(const std::string& InStr)
    {
        WCHAR buffer[512];
        MultiByteToWideChar(CP_ACP, 0, InStr.c_str(), -1, buffer, 512);
        return std::wstring(buffer);
    }

    std::wstring GetShaderAssetFullPath(const std::wstring& InAssetName)
    {
        return ShaderPath + InAssetName;
    }

    std::wstring GetAssetFullPath(const std::wstring& InAssetName)
    {
        return AssetPath + InAssetName;
    }

    std::vector<char> GetBinaryData(const std::wstring& InPath)
    {
        std::ifstream file(InPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file!");
        }

        size_t size = (size_t)file.tellg();

        std::vector<char> buffer(size);
        {
            file.seekg(0, std::ios::beg);
            file.read(&buffer[0], size);
            file.close();
        }
        return buffer;
    }
};