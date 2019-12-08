#pragma once

#include "MathHelper.h"
#include "FrameResource.h"

// test
#include "D3D12Commands.h"
#include "D3D12SwapChain.h"

using namespace DirectX;

// 이거 종류별로 나눠야함
// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem
{
	RenderItem() = default;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class D3D12RenderInterface
{
public:
	D3D12RenderInterface(class D3D12Device* pDevice);
	~D3D12RenderInterface();

	void CreateFrameResources();
	void DrawRenderItems(class D3D12CommandList* InCommandList);
	void OnResize(class D3D12CommandList* InCommandList);

/*

void FD3D12CommandContext::RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
{
	CommitGraphicsResourceTables();
	CommitNonComputeShaderConstants();

	uint32 VertexCount = GetVertexCountForPrimitiveCount(NumPrimitives, StateCache.GetGraphicsPipelinePrimitiveType());

	NumInstances = FMath::Max<uint32>(1, NumInstances);
	numDraws++;
	numPrimitives += NumInstances * NumPrimitives;
	if (bTrackingEvents)
	{
		GetParentDevice()->RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
	}

	StateCache.ApplyState<D3D12PT_Graphics>(); // RSSetViewports 이런애들 들어가있는거
	CommandListHandle->DrawInstanced(VertexCount, NumInstances, BaseVertexIndex, 0);

#if UE_BUILD_DEBUG
	OwningRHI.DrawCount++;
#endif
	DEBUG_EXECUTE_COMMAND_LIST(this);
}

파라미터로 commandlist를 넣어서 처리하도록 함
굳이 불러와서 commmandlist에 넣어주는게 아니라
*/

	// 필수
	//void Update();
	class D3D12Device* GetDevice() { return Device; }
	class D3D12SwapChain* GetSwapChain() { return SwapChain; }
	//class D3D12Descriptor* GetRenderTarget() { return RenderTargetDesc; }
	//class D3D12Descriptor* GetDepthStencil() { return DepthStencilDesc; }
	//class D3D12CommandListExecutor* GetCommandListExecutor() { return CmdListExecutor; }
	void ExecuteCommandList(class D3D12CommandList* InCommandList)
	{
		CmdListExecutor->Execute(InCommandList);
	}
	void FlushCommandQueue()
	{
		CmdListExecutor->FlushCommands();
	}
	void GetDescriptorHandleIncrementSize()
	{
		ShaderResourceDesc->SetSize(Device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	}

	// draw
	void UpdateViewport(class D3D12CommandList* InCommandList)
	{
		if (InCommandList && SwapChain)
		{
			InCommandList->Get()->RSSetViewports(1, &SwapChain->GetViewport());
			InCommandList->Get()->RSSetScissorRects(1, &SwapChain->GetRect());
		}
	}
	void SetViewport(class D3DViewportResource& InViewResource)
	{
		if (SwapChain)
		{
			D3D12_VIEWPORT& viewport = SwapChain->GetViewport();

			viewport.TopLeftX = InViewResource.TopLeftX;
			viewport.TopLeftY = InViewResource.TopLeftY;
			viewport.Width = InViewResource.Width;
			viewport.Height = InViewResource.Height;
			viewport.MinDepth = InViewResource.MinDepth;
			viewport.MaxDepth = InViewResource.MaxDepth;

			D3D12_RECT& ScissorRect = SwapChain->GetRect();

			ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const
	{
		return SwapChain->GetCurrentBackBufferView();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
	{
		if (DepthStencilDesc)
			return DepthStencilDesc->GetDescriptor()->GetCPUDescriptorHandleForHeapStart();

		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}

	void ReadyToRenderTarget(class D3D12CommandList* InCommandList)
	{
		if (InCommandList)
		{
			auto CommandList = InCommandList->Get();
			if (CommandList)
			{
				// Indicate a state transition on the resource usage.
				InCommandList->ResourceBarrier(SwapChain->GetCurrentBackBuffer(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

				// Clear the back buffer and depth buffer.
				CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
				CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

				// Specify the buffers we are going to render to.
				CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());
			}
		}
	}
	void FinishToRenderTarget(class D3D12CommandList* InCommandList)
	{
		// Indicate a state transition on the resource usage.
		InCommandList->ResourceBarrier(SwapChain->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}
	void SwapBackBufferToFrontBuffer()
	{
		if (SwapChain)
		{
			SwapChain->SwapBackBufferToFrontBuffer();
		}
	}

private:	
	// test -> virtual
//	class D3D12Descriptor* CreateRenderTarget();
	class D3D12Descriptor* CreateDepthStencil(class D3D12CommandList* InCommandList);
	class D3D12Descriptor* CreateShaderBuffer();

// 	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const
// 	{
// 		return SwapChain->GetCurrentBackBufferView();
// 	}
// 
// 	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
// 	{
// 		if (DepthStencilDesc)
// 			return DepthStencilDesc->GetDescriptor()->GetCPUDescriptorHandleForHeapStart();
// 
// 		return D3D12_CPU_DESCRIPTOR_HANDLE();
// 	}

private:
	// frame resources
	//std::unique_ptr<FrameResource> CurFrameResource;

	// Render items divided by PSO.
	std::vector<RenderItem*> OpaqueRitems;

// 	// List of all the render items.
// 	std::vector<std::unique_ptr<RenderItem>> AllRitems;

	class D3D12Device* Device = nullptr;
	class D3D12SwapChain* SwapChain = nullptr;
	class D3D12CommandListExecutor* CmdListExecutor = nullptr;

	//class D3D12Descriptor* RenderTargetDesc = nullptr;
	class D3D12Descriptor* ShaderResourceDesc = nullptr;
	class D3D12Descriptor* DepthStencilDesc = nullptr;
	class D3D12Resource* DepthStencilBuffer = nullptr;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	class D3D12Fence* Fence = nullptr;
};

