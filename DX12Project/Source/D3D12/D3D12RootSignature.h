#pragma once
#include <array>
#include "D3D12Device.h"

class D3D12RootSignature : public D3D12Api
{
public:
	D3D12RootSignature() = delete;
	explicit D3D12RootSignature(D3D12Device* InDevice, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& InDesc);
	explicit D3D12RootSignature(D3D12Device* InDevice, const D3D12_ROOT_SIGNATURE_DESC& InDesc);
	virtual ~D3D12RootSignature();

	ID3D12RootSignature* GetInterface() { return RootSignature.Get(); }
	ID3D12RootSignature** GetAddressOf();

	void InitTable(D3D12_DESCRIPTOR_RANGE_TYPE InType, D3D12_SHADER_VISIBILITY InVisibility, unsigned InCount);
	void InitConstBuffer();
	void InitShaderResource();

private:
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3DBlob> RootSignatureBlob = nullptr;

	// Root parameter can be a table, root descriptor or root constants.
	static constexpr unsigned MaxRootParameter = 32;
	std::array<CD3DX12_ROOT_PARAMETER1, MaxRootParameter> RootParameterSlots;

	unsigned RootParameterCount = 0;
	unsigned ConstBufferOffset = 0;
	unsigned ShaderResourceOffset = 0;
};