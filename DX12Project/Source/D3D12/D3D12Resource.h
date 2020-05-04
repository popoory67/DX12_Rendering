#pragma once
#include "D3DUtil.h"
#include "Texture.h"
#include "Material.h"
#include "d3dx12.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"

using namespace Microsoft::WRL;

class D3D12Resource
{
public:
	D3D12Resource() {}
	virtual ~D3D12Resource() { ReleaseCom(Resource); }

	ComPtr<ID3D12Resource> Get() { return Resource; }
	ID3D12Resource** GetAddressOf() { return Resource.GetAddressOf(); }
	ID3D12Resource* GetInterface() { return Resource.Get(); }
	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> GetGPUVirtualAddress() { return Resource->GetGPUVirtualAddress(); }
	D3D12_RESOURCE_DESC GetDesc() { return Resource->GetDesc(); }

	void Reset();

protected:
	ComPtr<struct ID3D12Resource> Resource = nullptr;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12DefaultResource : public D3D12Resource
{
public:
	D3D12DefaultResource() {}
	virtual ~D3D12DefaultResource() {}

	D3D12Resource* GetUploadResource() { return UploadResource; }

private:
	D3D12Resource* UploadResource = nullptr; // cpu to gpu
};

// -------------------------------------------------------------------------------------------------------------------- //
template<typename T>
class D3D12UploadResource : public D3D12Resource
{
public:
	D3D12UploadResource() = delete;
	D3D12UploadResource(bool InIsConstantBuffer = false) :
		IsConstantBuffer(InIsConstantBuffer)
	{
		ElementByteSize = sizeof(T);

		//CreateBuffer(InElementCount);
	}
	D3D12UploadResource(const D3D12UploadResource& rhs) = delete;
	D3D12UploadResource& operator=(const D3D12UploadResource& rhs) = delete;
	virtual ~D3D12UploadResource()
	{
		UnMap();
	}

	void SetTypeSize(UINT InSize) { ElementByteSize = InSize; }
	UINT GetTypeSize() { return ElementByteSize; }
	bool IsConstBuffer() { return IsConstantBuffer; }

	void Map()
	{
		ThrowIfFailed(Resource->Map(0, nullptr, reinterpret_cast<void**>(&MappedData)));
	}

	void UnMap()
	{
		if (Resource != nullptr)
			Resource->Unmap(0, nullptr);

		MappedData = nullptr;
	}

	//void CreateBuffer(UINT InElementCount)
	//{
	//	// Constant buffer elements need to be multiples of 256 bytes.
	//	// This is because the hardware can only view constant data 
	//	// at m*256 byte offsets and of n*256 byte lengths. 
	//	// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
	//	// UINT64 OffsetInBytes; // multiple of 256
	//	// UINT   SizeInBytes;   // multiple of 256
	//	// } D3D12_CONSTANT_BUFFER_VIEW_DESC;

	//	if (IsConstantBuffer)
	//		ElementByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(T));

	//	if (InElementCount > 0)
	//	{
	//		Device->CreateCommittedResource(
	//			Resource,
	//			D3D12_HEAP_TYPE_UPLOAD,
	//			D3D12_HEAP_FLAG_NONE,
	//			ElementByteSize * InElementCount,
	//			D3D12_RESOURCE_STATE_GENERIC_READ);

	//		ThrowIfFailed(Resource->Map(0, nullptr, reinterpret_cast<void**>(&MappedData)));
	//	}

	//	// We do not need to unmap until we are done with the resource.  However, we must not write to
	//	// the resource while it is in use by the GPU (so we must use synchronization techniques).
	//}

	ID3D12Resource* GetResource() const
	{
		return Resource.Get();
	}

	void CopyData(int InIndex, const T& InData)
	{
		memcpy(&MappedData[InIndex * ElementByteSize], &InData, sizeof(T));
	}

private:
	BYTE* MappedData = nullptr;

	UINT ElementByteSize = 0;
	bool IsConstantBuffer = false;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12RenderTargetResource : public D3D12Resource
{
public:
	D3D12RenderTargetResource() {}
	//D3D12RenderTargetResource(CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize);
	//D3D12RenderTargetResource(class D3D12SwapChain* InSwapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize, unsigned int InIndex);

	virtual ~D3D12RenderTargetResource() {}
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12DepthStencilResource : public D3D12Resource
{
public:
	D3D12DepthStencilResource();
	virtual ~D3D12DepthStencilResource() {}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
	DXGI_FORMAT GetDepthStencilFormat() { return DepthStencilFormat; }

private:
	class D3D12Descriptor* DepthStencilDesc = nullptr;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12ShaderResource : public D3D12Resource
{
public:
	D3D12ShaderResource() {}
	virtual ~D3D12ShaderResource() {}

	class D3D12Descriptor* GetDescriptor() { return ShaderResourceDesc; }
//	void LoadTextures(std::string InName = nullptr, std::wstring InFilePath = nullptr);
	D3D12Resource* GetTextureResource() { return UploadTextureResource; }

	UINT64 GetDescriptorHandleIncrementSize();

private:
	class D3D12Descriptor* ShaderResourceDesc = nullptr;

	Texture* Texture = nullptr;
	D3D12Resource* UploadTextureResource = nullptr;

//	std::unique_ptr<MaterialData> Material;
};

// -------------------------------------------------------------------------------------------------------------------- //