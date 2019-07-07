#pragma once
#include "D3DUtil.h"

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* pDevice, UINT elementCount, bool isConstantBuffer) :
		IsConstantBuffer(isConstantBuffer)
	{
		ElementByteSize = sizeof(T);

		// Constant buffer elements need to be multiples of 256 bytes.
		// This is because the hardware can only view constant data 
		// at m*256 byte offsets and of n*256 byte lengths. 
		// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
		// UINT64 OffsetInBytes; // multiple of 256
		// UINT   SizeInBytes;   // multiple of 256
		// } D3D12_CONSTANT_BUFFER_VIEW_DESC;
		if (isConstantBuffer)
			ElementByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(T));

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize*elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&Buffer)));

		ThrowIfFailed(Buffer->Map(0, nullptr, reinterpret_cast<void**>(&MappedData)));

		// We do not need to unmap until we are done with the resource.  However, we must not write to
		// the resource while it is in use by the GPU (so we must use synchronization techniques).
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (Buffer != nullptr)
			Buffer->Unmap(0, nullptr);

		MappedData = nullptr;
	}

	ID3D12Resource* Resource()const
	{
		return Buffer.Get();
	}

	void CopyData(int elementIndex, const T& data)
	{
		memcpy(&MappedData[elementIndex*ElementByteSize], &data, sizeof(T));
	}

private:
	ComPtr<ID3D12Resource> Buffer;
	BYTE* MappedData = nullptr;

	UINT ElementByteSize = 0;
	bool IsConstantBuffer = false;
};
