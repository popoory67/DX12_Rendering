#pragma once
#include <d3d12.h>
#include <array>
#include "d3dx12.h"

using namespace Microsoft::WRL;

// 여러개 가능한지 테스트
// parameter가 array로 관리 가능한지 테스트
class D3D12RootSignature
{
public:
	D3D12RootSignature();
	~D3D12RootSignature();

	ID3D12RootSignature* GetInterface() { return RootSignature.Get(); }
	ID3D12RootSignature** GetAddressOf();

	void InitTable(D3D12_DESCRIPTOR_RANGE_TYPE InType, D3D12_SHADER_VISIBILITY InVisibility, unsigned InCount);
	void InitConstBuffer();
	void InitShaderResource();

	void SetRootSignature(class D3D12Device* InDevice);

private:
	void AddParam(CD3DX12_ROOT_PARAMETER& InParam);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;

	// Root parameter can be a table, root descriptor or root constants.
	std::vector<CD3DX12_ROOT_PARAMETER> RootParameterSlots;

	unsigned TableCount = 0;
	unsigned ConstBufferOffset = 0;
	unsigned ShaderResourceOffset = 0;
};