#include "MaterialComponent.h"
#include "Material.h"
#include "MathHelper.h"
#include "PrimitiveComponent.h"
#include "Util.h"

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
    GetProxy().ShaderInfo_VS.Hash = ShaderCode_VS_Hash;

    GetProxy().ShaderInfo_FS.Bytecode = reinterpret_cast<BYTE*>(ShaderCode_FS.data());
    GetProxy().ShaderInfo_FS.BytecodeLength = ShaderCode_FS.size();
    GetProxy().ShaderInfo_FS.Hash = ShaderCode_FS_Hash;

    GetProxy().TextureInfo = &Settings;
}

MaterialProxy& MaterialComponent::GetProxy() const
{
    return *Proxy;
}

void MaterialComponent::SetTexture(const std::wstring& InAssetName)
{
    LoadTexture(Util::GetAssetFullPath(InAssetName));
}

void MaterialComponent::SetShader(const std::wstring& InAssetName, ShaderType InShaderType)
{
    if (InShaderType == ShaderType::Vertex)
    {
        ShaderCode_VS = LoadShader(Util::GetShaderAssetFullPath(InAssetName));
        ShaderCode_VS_Hash = std::hash<std::wstring>()(InAssetName);
    }
    else if (InShaderType == ShaderType::Fragment)
    {
        ShaderCode_FS = LoadShader(Util::GetShaderAssetFullPath(InAssetName));
        ShaderCode_FS_Hash = std::hash<std::wstring>()(InAssetName);
    }
}

void MaterialComponent::LoadTexture(const std::wstring& InPath)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &InPath[0], (int)InPath.size(), NULL, 0, NULL, NULL);
    std::string strTo(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &InPath[0], (int)InPath.size(), &strTo[0], size, NULL, NULL);

    int width, height, channels;
    Settings.TextureData = stbi_load(strTo.c_str(), &width, &height, &channels, 0);
    if (!Settings.TextureData)
    {
        // TODO
        // Print a warning message it doesn't have a texture file. 
        Settings.TextureData = new BYTE{};
        Settings.Width = 1;
        Settings.Height = 1;
        Settings.Channels = 3;
    }
    else
    {
        Settings.Width = width;
        Settings.Height = height;
        Settings.Channels = channels;
    }
}

std::vector<char> MaterialComponent::LoadShader(const std::wstring& InPath)
{
    return Util::GetBinaryData(InPath);
}
