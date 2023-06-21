#pragma once
#include "D3D12Device.h"

//#include "D3DUtil.h"

struct D3D12ResourceDesc : public D3D12_RESOURCE_DESC
{
	D3D12ResourceDesc() = default;
	D3D12ResourceDesc(const CD3DX12_RESOURCE_DESC& InDesc)
		: D3D12_RESOURCE_DESC(InDesc)
	{
	}

	D3D12ResourceDesc(const D3D12_RESOURCE_DESC& InDesc)
		: D3D12_RESOURCE_DESC(InDesc)
	{
	}

};

class D3D12Resource : public D3D12Api, public std::enable_shared_from_this<D3D12Resource>
{
public:
	D3D12Resource() = delete;
	D3D12Resource(D3D12Device* InDevice, ID3D12Resource* InResource);
	virtual ~D3D12Resource();

	ComPtr<ID3D12Resource>& Get() noexcept { return Resource; }
	ID3D12Resource** GetAddressOf() noexcept { return Resource.GetAddressOf(); }
	ID3D12Resource* GetResource() noexcept { return Resource.Get(); }
	
	D3D12_RESOURCE_DESC GetDesc() const { return Desc; }
	D3D12_RESOURCE_STATES GetResourceState() const { return ResourceState; }

	void FORCEINLINE SetResourceState(const D3D12_RESOURCE_STATES& InNewState)
	{
		ResourceState = InNewState; 
	}

	void Reset();

	void Map();
	void Unmap();

protected:
	ComPtr<ID3D12Resource> Resource = nullptr;
	
	D3D12ResourceDesc Desc;
	D3D12_RESOURCE_STATES ResourceState;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12ResourceLocation : public D3D12Api
{
public:
	D3D12ResourceLocation() = delete;
	explicit D3D12ResourceLocation(D3D12Device* InDevice);
	virtual ~D3D12ResourceLocation() = default;

	void SetResource(D3D12Resource* InResource);
	D3D12Resource& GetResource() { return *Resource; }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
	{
		return Resource->Get()->GetGPUVirtualAddress(); 
	}

	uint64_t GetSize() { return Size; }
	
private:
	D3D12Resource* Resource = nullptr;
	
	// This is only for D3D12_RESOURCE_DIMENSION_BUFFER
	//D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress;
	uint64_t Size = 0;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12VertexBuffer : public D3D12Api
{
public:
	D3D12VertexBuffer() = delete;
	explicit D3D12VertexBuffer(D3D12Device* InDevice);
	virtual ~D3D12VertexBuffer() = default;

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return ResourceLocation->GetGPUVirtualAddress(); }
	uint64_t GetSize() { return ResourceLocation->GetSize(); }
	
private:
	D3D12ResourceLocation* ResourceLocation = nullptr;
};

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12DefaultResource : public D3D12Api
{
public:
	D3D12DefaultResource() = delete;
	explicit D3D12DefaultResource(D3D12Device* InDevice);
	virtual ~D3D12DefaultResource() { SafeDelete(Resource); }

	D3D12Resource* GetResource() { return Resource; }

private:
	D3D12Resource* Resource = nullptr; // cpu to gpu
};

// -------------------------------------------------------------------------------------------------------------------- //
template<typename T>
class D3D12UploadResource : public D3D12DefaultResource
{
public:
	D3D12UploadResource() = delete;
	D3D12UploadResource(D3D12Device* InDevice, bool InIsConstantBuffer = false);
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

template<typename T>
inline D3D12UploadResource<T>::D3D12UploadResource(D3D12Device* InDevice, bool InIsConstantBuffer)
	: D3D12DefaultResource(InDevice)
	, IsConstantBuffer(InIsConstantBuffer)
{
	ElementByteSize = sizeof(T);

	//CreateBuffer(InElementCount);
}

// -------------------------------------------------------------------------------------------------------------------- //

class D3D12ShaderResource : public D3D12Api
{
public:
	D3D12ShaderResource() = delete;
	explicit D3D12ShaderResource(D3D12Device* InDevice);
	virtual ~D3D12ShaderResource() = default;

	inline class D3D12Descriptor* GetDescriptor() { return ShaderResourceDesc; }
//	void LoadTextures(std::string InName = nullptr, std::wstring InFilePath = nullptr);
	inline D3D12Resource* GetResource() { return UploadTextureResource; }

	UINT64 GetDescriptorHandleIncrementSize();

private:
	class D3D12Descriptor* ShaderResourceDesc = nullptr;

	class Texture* Texture = nullptr;
	D3D12Resource* UploadTextureResource = nullptr;

//	std::unique_ptr<MaterialData> Material;
};

// -------------------------------------------------------------------------------------------------------------------- //
