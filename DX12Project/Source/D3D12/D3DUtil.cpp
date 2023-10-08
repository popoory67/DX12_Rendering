#include "D3DUtil.h"
#include <comdef.h>
#include "d3dx12.h"
#include "D3D12Resource.h"

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
	ErrorCode(hr),
	FunctionName(functionName),
	Filename(filename),
	LineNumber(lineNumber)
{
}

std::wstring DxException::ToString()const
{
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}

D3DUtil::D3DUtil()
{
}


D3DUtil::~D3DUtil()
{
}

ComPtr<ID3DBlob> D3DUtil::CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}

namespace D3D12
{
	D3D12_SHADER_BYTECODE CompileShader(const std::wstring& InFilename, const D3D_SHADER_MACRO* InShaderMacros, const std::string& InEntrypoint, const std::string& InTarget)
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
		{
			OutputDebugStringA((char*)errors->GetBufferPointer());
		}

		ThrowIfFailed(hr);

		return CD3DX12_SHADER_BYTECODE(byteCode->GetBufferPointer(), byteCode->GetBufferSize());
	}
};