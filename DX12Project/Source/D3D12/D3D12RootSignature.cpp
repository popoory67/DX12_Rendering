#include "D3D12RootSignature.h"
#include "D3D12Device.h"
#include "D3DUtil.h"

D3D12RootSignature::D3D12RootSignature(D3D12Device* InDevice, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& InDesc)
	: D3D12Api(InDevice)
{
    ComPtr<ID3DBlob> error;
	const D3D_ROOT_SIGNATURE_VERSION version = InDevice->GetRootSignatureVersion();

    HRESULT serializeHR = D3DX12SerializeVersionedRootSignature(&InDesc, version, &RootSignatureBlob, &error);
    if (error)
    {
        ::OutputDebugStringA((char*)error->GetBufferPointer());
        ThrowIfFailed(serializeHR);
    }

    ThrowIfFailed(GetDevice()->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&
        RootSignature)));
}

D3D12RootSignature::D3D12RootSignature(D3D12Device* InDevice, const D3D12_ROOT_SIGNATURE_DESC& InDesc)
    : D3D12Api(InDevice)
{
    ComPtr<ID3DBlob> error;
    const D3D_ROOT_SIGNATURE_VERSION version = InDevice->GetRootSignatureVersion();

    HRESULT serializeHR = D3D12SerializeRootSignature(&InDesc, /*version*/D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureBlob, &error);
    if (error)
    {
        ::OutputDebugStringA((char*)error->GetBufferPointer());
        ThrowIfFailed(serializeHR);
    }

    ThrowIfFailed(GetDevice()->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&
        RootSignature)));
}

D3D12RootSignature::~D3D12RootSignature()
{
	RootSignature.Reset();
	RootSignatureBlob.Reset();
}

ID3D12RootSignature** D3D12RootSignature::GetAddressOf()
{
	return RootSignature.GetAddressOf();
}

void D3D12RootSignature::InitTable(D3D12_DESCRIPTOR_RANGE_TYPE InType, D3D12_SHADER_VISIBILITY InVisibility, unsigned InCount)
{
	CD3DX12_DESCRIPTOR_RANGE1 table;
	table.Init(InType, InCount, 0, 0);

	RootParameterSlots[RootParameterCount].InitAsDescriptorTable(1, &table, InVisibility); // 1 : table count
	++RootParameterCount;
}

void D3D12RootSignature::InitConstBuffer()
{
    RootParameterSlots[RootParameterCount].InitAsConstantBufferView(ConstBufferOffset);
    ++RootParameterCount;
	++ConstBufferOffset;
}

void D3D12RootSignature::InitShaderResource()
{
    RootParameterSlots[RootParameterCount].InitAsShaderResourceView(ShaderResourceOffset, 1);
    ++RootParameterCount;
	++ShaderResourceOffset;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3D12RootSignature::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

