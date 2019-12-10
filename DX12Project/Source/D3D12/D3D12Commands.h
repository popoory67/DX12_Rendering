#pragma once
#include "CommnadList.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <stdint.h>
#include <comdef.h>
#include "d3dx12.h"
#include <DirectXColors.h>
#include "D3DUtil.h"
#include "GameTimer.h"
#include "FrameResource.h"
#include "D3D12Resource.h"
#include "D3D12Descriptor.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

class D3D12CommandList : public CommandListBase
{
public:
	D3D12CommandList() = delete;
	D3D12CommandList(class D3D12Device* InD3D12Device);

	ComPtr<ID3D12GraphicsCommandList>& Get() { return CommandList; }
	ID3D12GraphicsCommandList* GetGraphics() { return CommandList.Get(); }
	ID3D12CommandList* GetCommandLists();

	// Indicate a state transition on the resource usage.
	void SetResourceTransition(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InPrevState, D3D12_RESOURCE_STATES InNextState);
	void ResourceBarrier(class D3D12Resource* InResource, D3D12_RESOURCE_STATES InFrom, D3D12_RESOURCE_STATES InTo);

	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect = nullptr);
	void SetRenderTargets(UINT InNumRenderTargetDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilDescriptorHandle);

	// test
	void SetDescriptor(class D3D12Descriptor* InDescriptor)
	{
		if (InDescriptor)
		{
			// heap이랑 root랑 같이 묶어야함
			ID3D12DescriptorHeap* descriptorHeaps[] = { InDescriptor->GetDescriptor().Get() };
			CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			CommandList->SetGraphicsRootSignature(RootSignature.Get());
		}
	}

	void Reset();

	// 원래는 base에 있어야하는데 D3D12용 인터페이스라서 일단 여기에 둠
protected:
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap = nullptr;
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;

	//ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)
	//std::vector<RenderItem*> RenderItems;

	ConstantResource<ObjectConstants>* ObjectBuffer;
	ConstantResource<PassConstants>* MainPassBuffer;
	ConstantResource<MaterialConstants>* MaterialBuffer;

private:
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandListAllocator;
};

class D3D12CommandListExecutor : public CommandListExecutor
{
public:
	D3D12CommandListExecutor() = delete;
	D3D12CommandListExecutor(class D3D12Device* InD3D12Device);

	void Execute(CommandListBase& InCommandList) override {}
	void Execute(class D3D12CommandList* InCommandList);
	void FlushCommands() override;

	ComPtr<ID3D12CommandQueue>& GetExecutor() { ReturnCheckAssert(CommandQueue); }
	ID3D12CommandQueue* GetExecutorInterface() { ReturnCheckAssert(CommandQueue.Get()); }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;
	//ComPtr<ID3D12Fence> Fence;
	class D3D12Fence* Fence = nullptr;

	UINT64 CurrentFenceCount = 0;
};

// 여기부터 command
// struct D3D12ViewportCommand : public CommandBase<D3D12ViewportCommand>
// {
// 	D3D12ViewportCommand() {}
// 	virtual ~D3D12ViewportCommand() {}
// 
// 	virtual void Execute(CommandListBase& InCmdList)
// 	{
// 		D3D12CommandList* pList = static_cast<class D3D12CommandList*>(&InCmdList);
// 		if (pList)
// 		{
// 			ComPtr<ID3D12GraphicsCommandList> pD3d12CommantList = pList->Get();
// 
// 			pD3d12CommantList->RSSetViewports(1, &ScreenViewport);
// 		}
// 	}
// 
// private:
// 	D3D12_VIEWPORT ScreenViewport;
// };
