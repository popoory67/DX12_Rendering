#pragma once
#include "D3D12RenderInterface.h"

class RenderInterface
{
public:
	RenderInterface() = delete;
	RenderInterface(class D3D12RenderInterface* InInterface);
	~RenderInterface();

	static class D3D12RenderInterface* GetInterface();
	void SetInterface(class D3D12RenderInterface* InInterface);

	static void CreateResource(class D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc = {}, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	static void CreateResource(class D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	static void CreateDefaultBuffer(class D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize);

	//template<typename Type>
	//void CreateUploadResource(class D3D12UploadResource<Type>* InResource, UINT InElementCount)
	//{
	//	GetInterface()->CreateUploadResource<Type>(InResource, InElementCount);
	//}

private:

	static class D3D12RenderInterface* CurrentInterface;
};