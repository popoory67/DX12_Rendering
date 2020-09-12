#pragma once
#include "D3D12Commands.h"
#include "D3D12Device.h"

// Management for command lists
class D3D12RenderInterface : public D3D12DeviceChild
{
public:
	D3D12RenderInterface() = delete;
	D3D12RenderInterface(class D3D12DeviceChild* InDevice);
	virtual ~D3D12RenderInterface();

	class D3D12CommandList& GetCommandList() const
	{
		return *GetParent()->GetCommandList();
	}
	
	void ExecuteCommandList();
	void FlushCommandQueue() const;
	//void UpdateViewport(class D3D12Viewport* InSwapChain);
	//void SetViewport(class D3DViewportResource& InViewResource, class D3D12Viewport* InSwapChain);
	//void ReadyToRenderTarget(class D3D12Resource* InBackBuffer, D3D12_CPU_DESCRIPTOR_HANDLE InBackBufferView, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilBuffer);
	//void FinishToRenderTarget(class D3D12Resource* InBackBuffer);
	void ResetCommandList();

	void CreateResource(class D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc = {}, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateResource(class D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateDefaultBuffer(class D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize);
	void CreateRenderTarget(class D3D12Resource* InResource, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize);
	void CreateShaderResource(class D3D12ShaderResource* InResource, class D3D12Descriptor* InDescriptor, std::string InName = nullptr, std::wstring InFilePath = nullptr);
	
	template<typename Type>
	void CreateUploadResource(class D3D12UploadResource<Type>* InResource, UINT InElementCount)
	{
		if (InResource->IsConstBuffer())
			InResource->SetTypeSize(D3DUtil::CalcConstantBufferByteSize(sizeof(Type)));

		if (InElementCount > 0)
		{
			GetParent()->CreateCommittedResource(
				InResource,
				D3D12_HEAP_TYPE_UPLOAD,
				D3D12_HEAP_FLAG_NONE,
				InResource->GetTypeSize() * InElementCount,
				D3D12_RESOURCE_STATE_GENERIC_READ);

			InResource->Map();
		}
	}

	void LoadTexture(class D3D12ShaderResource* InResource, std::string InName = nullptr, std::wstring InFilePath = nullptr);

private:

	class D3D12Fence* Fence = nullptr;

	class D3D12PipelineState* PipelineState;
};