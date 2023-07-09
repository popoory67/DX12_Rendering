#pragma once
#include <d3d12.h>

using namespace Microsoft::WRL;

class D3D12BinaryLargeObject
{
public:
	D3D12BinaryLargeObject();
	D3D12BinaryLargeObject(const std::wstring& InFilename, const D3D_SHADER_MACRO* InShaderMacros, const std::string& InEntrypoint, const std::string& InTarget);
	~D3D12BinaryLargeObject();

	ComPtr<ID3DBlob> Get() { return BinaryLargeObject; }

	D3D12_SHADER_BYTECODE GetShaderBytecode() const;

	void CreateBlob(const UINT InByteSize = 0);

protected:
	void CompileShader(const std::wstring& InFilename, const D3D_SHADER_MACRO* InShaderMacros, const std::string& InEntrypoint, const std::string& InTarget);

private:
	ComPtr<ID3DBlob> BinaryLargeObject = nullptr;
};

class D3D12VertexShaderObject : public D3D12BinaryLargeObject
{
public:
	D3D12VertexShaderObject();
	D3D12VertexShaderObject(const std::wstring& InFilename);
	~D3D12VertexShaderObject();
};

class D3D12PixelShaderObject : public D3D12BinaryLargeObject
{
public:
	D3D12PixelShaderObject();
	D3D12PixelShaderObject(const std::wstring& InFilename);
	~D3D12PixelShaderObject();
};