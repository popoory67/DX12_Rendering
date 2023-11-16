#include "MaterialComponent.h"
#include "Material.h"
#include "MathHelper.h"
#include "PrimitiveComponent.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

MaterialProxy::MaterialProxy()
{
    ShaderInfo_VS.Type = ShaderType::Vertex;
    ShaderInfo_FS.Type = ShaderType::Fragment;
}

MaterialComponent::MaterialComponent(class Scene* InScene, Component* InParent)
    : Parent(InScene, InParent)
{

}

MaterialComponent::~MaterialComponent()
{
    stbi_image_free(Settings.TextureData);
    Settings.TextureData = nullptr;

    SafeDelete(Proxy);
}

void MaterialComponent::Initialize()
{
    Proxy = new MaterialProxy();
}

void MaterialComponent::UpdateResources()
{
    GetProxy().ShaderInfo_VS.Bytecode = reinterpret_cast<BYTE*>(ShaderCode_VS.data());
    GetProxy().ShaderInfo_VS.BytecodeLength = ShaderCode_VS.size();

    GetProxy().ShaderInfo_FS.Bytecode = reinterpret_cast<BYTE*>(ShaderCode_FS.data());
    GetProxy().ShaderInfo_FS.BytecodeLength = ShaderCode_FS.size();

    GetProxy().TextureInfo = &Settings;
}

MaterialProxy& MaterialComponent::GetProxy() const
{
    return *Proxy;
}

void MaterialComponent::SetTexture(const std::wstring& InPath)
{
    LoadTexture(InPath.c_str());
}

void MaterialComponent::SetShader(const std::wstring& InPath, ShaderType InShaderType)
{
    if (InShaderType == ShaderType::Vertex)
    {
        ShaderCode_VS = LoadShader(InPath);
    }
    else if (InShaderType == ShaderType::Fragment)
    {
        ShaderCode_FS = LoadShader(InPath);
    }
}

void MaterialComponent::LoadTexture(const std::wstring& InPath)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &InPath[0], (int)InPath.size(), NULL, 0, NULL, NULL);
    std::string strTo(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &InPath[0], (int)InPath.size(), &strTo[0], size, NULL, NULL);

    int width, height, channels;
    Settings.TextureData = stbi_load(strTo.c_str(), &width, &height, &channels, 0);
    Settings.Width = width;
    Settings.Height = height;
    Settings.Channels = channels;
}

std::vector<char> MaterialComponent::LoadShader(const std::wstring& InPath)
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
