#pragma once
#include "D3DUtil.h"

using namespace Microsoft::WRL;

class D3D12Resource
{
public:
	D3D12Resource();
	D3D12Resource(class D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue);
	D3D12Resource(class D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue);
	virtual ~D3D12Resource() { ReleaseCom(Resource); }

	ComPtr<ID3D12Resource>& Get() { return Resource; }
	ID3D12Resource* GetInterface() { return Resource.Get(); }
	void Reset();

protected:
	void CreateResource(class D3D12Device* InDevice, D3D12_RESOURCE_DESC InDesc, D3D12_CLEAR_VALUE InValue);
	void CreateResource(class D3D12Device* InDevice, UINT64 InByteSize, D3D12_CLEAR_VALUE InValue);

protected:
	ComPtr<struct ID3D12Resource> Resource = nullptr;
};

class D3D12RenderTargetResource : public D3D12Resource
{
public:
	D3D12RenderTargetResource() = delete;
//	D3D12RenderTargetResource(class D3D12Device* InDevice, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize); // swap에서만 사용하고있기 떄문에 일단은 봉인
	D3D12RenderTargetResource(class D3D12Device* InDevice, class D3D12SwapChain* InSwapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize, unsigned int InIndex);

	virtual ~D3D12RenderTargetResource() {}
};

class D3D12DepthStencilResource : public D3D12Resource
{
public:
	D3D12DepthStencilResource() = delete;
	D3D12DepthStencilResource(class D3D12Device* InDevice, class D3D12CommandList* InCommandList);

	virtual ~D3D12DepthStencilResource() {}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

private:
	class D3D12Descriptor* DepthStencilDesc = nullptr;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

struct TextureData
{
	// Unique material name for lookup.
	std::string Name;
	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

class D3D12ShaderResource : public D3D12Resource
{
public:
	D3D12ShaderResource() = delete;
	D3D12ShaderResource(class D3D12Device* InDevice, class D3D12CommandList* InCommandList = nullptr, std::string InName = nullptr, std::wstring InFilePath = nullptr);

	virtual ~D3D12ShaderResource() {}

	void LoadTextures(class D3D12Device* InDevice, class D3D12CommandList* InCommandList, std::string InName = nullptr, std::wstring InFilePath = nullptr);

	UINT GetDescriptorHandleIncrementSize();

private:
	class D3D12Descriptor* ShaderResourceDesc = nullptr;
	std::unique_ptr<TextureData> Texture;
};