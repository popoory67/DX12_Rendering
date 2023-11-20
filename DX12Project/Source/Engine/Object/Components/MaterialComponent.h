#pragma once
#include "RenderComponent.h"
#include "RenderResource.h"

struct MaterialProxy
{
	MaterialProxy();
	virtual ~MaterialProxy() = default;

	ShaderBinding ShaderInfo_VS;
	ShaderBinding ShaderInfo_FS;

	TextureSettings* TextureInfo = nullptr;
};

class MaterialComponent : public RenderComponent
{
	using Parent = RenderComponent;

public:
	MaterialComponent() = delete;
	MaterialComponent(class Scene* InScene, Component* InParent = nullptr);
	virtual ~MaterialComponent();

	void UpdateResources() override;

	MaterialProxy& GetProxy() const;

	void SetTexture(const std::wstring& InAssetName);
	void SetShader(const std::wstring& InAssetName, ShaderType InShaderType);

protected:
	void Initialize() override;

private:
	void LoadTexture(const std::wstring& InPath);
	std::vector<char> LoadShader(const std::wstring& InPath);

private:
	TextureSettings Settings;

	std::vector<char> ShaderCode_VS;
	size_t ShaderCode_VS_Hash;

	std::vector<char> ShaderCode_FS;
	size_t ShaderCode_FS_Hash;

	MaterialProxy* Proxy = nullptr;
};