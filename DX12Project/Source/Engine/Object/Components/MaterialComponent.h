#pragma once
#include "RenderComponent.h"

enum class ShaderType : unsigned int
{
	Vertex,
	Fragment,
};

struct ShaderBinding
{
	SIZE_T BytecodeLength;
	BYTE* Bytecode = nullptr;
	ShaderType Type;
};

class MaterialProxy
{
	friend class MaterialComponent;

public:
	MaterialProxy();
	virtual ~MaterialProxy() = default;

private:
	ShaderBinding ShaderInfo_VS;
	ShaderBinding ShaderInfo_FS;

	BYTE* TextureData = nullptr;
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

	void SetTexture(const std::wstring& InPath);
	void SetShader(const std::wstring& InPath, ShaderType InShaderType);

protected:
	void Initialize() override;

private:
	BYTE* LoadTexture(const std::wstring& InPath);
	std::vector<char> LoadShader(const std::wstring& InPath);

private:
	BYTE* TextureData;
	std::vector<char> ShaderCode_VS;
	std::vector<char> ShaderCode_FS;

	MaterialProxy* Proxy = nullptr;
};