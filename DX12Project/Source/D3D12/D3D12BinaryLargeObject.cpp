#include "stdafx.h"
#include "D3D12BinaryLargeObject.h"
#include "d3dcompiler.h"
#include "D3DUtil.h"

D3D12BinaryLargeObject::D3D12BinaryLargeObject()
{

}

D3D12BinaryLargeObject::D3D12BinaryLargeObject(const std::wstring& InFilename, const D3D_SHADER_MACRO* InShaderMacros, const std::string& InEntrypoint, const std::string& InTarget)
{
	CompileShader(InFilename, InShaderMacros, InEntrypoint, InTarget);
}

D3D12BinaryLargeObject::~D3D12BinaryLargeObject()
{

}

BYTE* D3D12BinaryLargeObject::GetBufferPointer() const
{
	assert(BinaryLargeObject);
	return reinterpret_cast<BYTE*>(BinaryLargeObject->GetBufferPointer());
}

SIZE_T D3D12BinaryLargeObject::GetBufferSize() const
{
	assert(BinaryLargeObject);
	return BinaryLargeObject->GetBufferSize();
}

ID3DBlob** D3D12BinaryLargeObject::GetAddressOf()
{
	return BinaryLargeObject.GetAddressOf();
}

void D3D12BinaryLargeObject::CompileShader(const std::wstring& InFilename, const D3D_SHADER_MACRO* InShaderMacros, const std::string& InEntrypoint, const std::string& InTarget)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	
	hr = D3DCompileFromFile(InFilename.c_str(), InShaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, InEntrypoint.c_str(), InTarget.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	BinaryLargeObject = byteCode;
}

D3D12VertexShaderObject::D3D12VertexShaderObject()
{
	CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
}

D3D12VertexShaderObject::D3D12VertexShaderObject(const std::wstring& InFilename)
{
	CompileShader(InFilename, nullptr, "VS", "vs_5_1");
}

D3D12VertexShaderObject::~D3D12VertexShaderObject()
{

}

D3D12PixelShaderObject::D3D12PixelShaderObject()
{
	CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
}

D3D12PixelShaderObject::D3D12PixelShaderObject(const std::wstring& InFilename)
{
	CompileShader(InFilename, nullptr, "PS", "ps_5_1");
}

D3D12PixelShaderObject::~D3D12PixelShaderObject()
{

}
