#include "stdafx.h"
#include "D3D12RootSignature.h"
#include "D3D12Device.h"
#include "D3D12BinaryLargeObject.h"

D3D12RootSignature::D3D12RootSignature(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{
	RootParameterSlots.clear();
}

D3D12RootSignature::~D3D12RootSignature()
{

}

ID3D12RootSignature** D3D12RootSignature::GetAddressOf()
{
	return RootSignature.GetAddressOf();
}

void D3D12RootSignature::InitTable(D3D12_DESCRIPTOR_RANGE_TYPE InType, D3D12_SHADER_VISIBILITY InVisibility, unsigned InCount)
{
	CD3DX12_DESCRIPTOR_RANGE table;
	table.Init(InType, InCount, 0, 0);

	CD3DX12_ROOT_PARAMETER param;
	param.InitAsDescriptorTable(1, &table, InVisibility); // 1 : table 개수
	// append를 쓰도록 하자 => 책 참고(7장)

	AddParam(param);

	++TableCount;
}

void D3D12RootSignature::InitConstBuffer()
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsConstantBufferView(ConstBufferOffset);

	AddParam(param);

	++ConstBufferOffset;
}

void D3D12RootSignature::InitShaderResource()
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsShaderResourceView(ShaderResourceOffset, 1);

	AddParam(param);

	++ShaderResourceOffset;
}

void D3D12RootSignature::SetRootSignature()
{
// 	CD3DX12_DESCRIPTOR_RANGE texTable;
// 	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	// Perfomance TIP: Order from most frequent to least frequent.
// 	SlotRootParameter[0].InitAsConstantBufferView(0); // b0
// 	SlotRootParameter[1].InitAsConstantBufferView(1); // b1
// 	SlotRootParameter[2].InitAsShaderResourceView(0, 1); // t0 space1
// 	SlotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

// 	SlotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
// 	SlotRootParameter[1].InitAsConstantBufferView(0);
// 	SlotRootParameter[2].InitAsConstantBufferView(1);
// 	SlotRootParameter[3].InitAsConstantBufferView(2);

	// test
	InitTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL, 2);
	InitConstBuffer();
	InitConstBuffer();
	InitConstBuffer();

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	UINT count = TableCount + ConstBufferOffset + ShaderResourceOffset;
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(count, &*RootParameterSlots.begin(), (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	D3D12BinaryLargeObject* pSerializedRootSig = new D3D12BinaryLargeObject();
	D3D12BinaryLargeObject* pErrorBlob = new D3D12BinaryLargeObject();

	if (pSerializedRootSig && pErrorBlob)
	{
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSerializedRootSig->GetAddressOf(), pErrorBlob->GetAddressOf());

		if (pErrorBlob->Get())
			::OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		ThrowIfFailed(hr);

		ThrowIfFailed(GetDevice()->CreateRootSignature(0, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())));
	}
}

void D3D12RootSignature::AddParam(CD3DX12_ROOT_PARAMETER& InParam)
{
	RootParameterSlots.emplace_back(InParam);
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

