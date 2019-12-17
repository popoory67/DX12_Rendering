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

	MaterialData* Mat = nullptr;
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
	D3D12RenderInterface() = delete;

	D3D12RenderInterface(class D3D12Device* InDevice, class D3D12CommandList* InCommandList);
	~D3D12RenderInterface();

	class D3D12Device* GetDevice() { return Device; }
	class D3D12SwapChain* GetSwapChain() { return SwapChain; }

	DXGI_FORMAT GetBackBufferFormat() const;
	DXGI_FORMAT GetDepthStencilFormat() const;

	// 미구현
	void CreateFrameResources();
	void DrawRenderItems(class D3D12CommandList* InCommandList);

	// 구현
	void OnResize(class D3D12CommandList* InCommandList);
	
	// draw
	void ExecuteCommandList(class D3D12CommandList* InCommandList) const;
	void FlushCommandQueue() const;
	void UpdateViewport(class D3D12CommandList* InCommandList);
	void SetViewport(class D3DViewportResource& InViewResource);
	void ReadyToRenderTarget(class D3D12CommandList* InCommandList);
	void FinishToRenderTarget(class D3D12CommandList* InCommandList);
	void SwapBackBufferToFrontBuffer();

	// data update
	//void Update();

private:
	class D3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;

private:
	// frame resources
	//std::unique_ptr<FrameResource> CurFrameResource;

	// Render items divided by PSO.
//	std::vector<RenderItem*> OpaqueRitems;

// 	// List of all the render items.
// 	std::vector<std::unique_ptr<RenderItem>> AllRitems;

	class D3D12Device* Device = nullptr;
	class D3D12SwapChain* SwapChain = nullptr;
	class D3D12CommandListExecutor* CmdListExecutor = nullptr;
	class D3D12DepthStencilResource* DepthStencilBuffer = nullptr;
	class D3D12Fence* Fence = nullptr;
	std::vector<class D3D12PipelineState*> PipelineStates;
};

