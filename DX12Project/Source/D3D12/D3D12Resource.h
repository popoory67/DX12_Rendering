#pragma once
#include <intsafe.h>

using namespace Microsoft::WRL;

// buffer
class D3D12Resource
{
public:
	D3D12Resource() {}
	virtual ~D3D12Resource() {}

	ComPtr<struct ID3D12Resource> Get() { return Resource; }

	void CreateResource(class D3D12Device* InDevice, struct D3D12_RESOURCE_DESC* InDesc = nullptr, struct D3D12_CLEAR_VALUE* InValue = nullptr);
	void CreateResource(class D3D12Device* InDevice, UINT64 InByteSize, struct D3D12_CLEAR_VALUE* InValue = nullptr);

private:
	ComPtr<struct ID3D12Resource> Resource = nullptr;
};