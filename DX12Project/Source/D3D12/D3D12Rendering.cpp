#include "stdafx.h"
#include "D3D12Rendering.h"
#include <combaseapi.h>
#include "GeometryGenerator.h"
#include "DDSTextureLoader.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12SwapChain.h"
#include "D3D12Commands.h"
#include "D3D12PipelineState.h"
#include "SceneRendering.h"
#include "Scene.h"

D3D12Renderer* D3D12Renderer::Instance = nullptr;

D3D12Renderer::D3D12Renderer()
{
	// Only one D3D12Renderer can be constructed.
	assert(Instance == nullptr);
	Instance = this;
}

D3D12Renderer::~D3D12Renderer()
{
}

D3D12Renderer& D3D12Renderer::GetInstance()
{
	if (!Instance)
		Instance = new D3D12Renderer();

	return *Instance;
}

bool D3D12Renderer::Initialize()
{
	Device.reset(new D3D12Device());
	assert(Device);

	RenderInterface.reset(new D3D12RenderInterface(Device.get()));
	assert(RenderInterface);

	Renderer.reset(new SceneRenderer());
	assert(Renderer);

	D3D12DeviceChild* DeviceChild = new D3D12DeviceChild(Device.get());
	assert(DeviceChild);

	SwapChain.reset(new D3D12SwapChain(DeviceChild));
	assert(SwapChain);

	SwapChain->Create(RenderInterface->GetCommandExecutor());
	SwapChain->OnResize();

	DepthStencilBuffer = new D3D12DepthStencilResource();

	// on resize
	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	OnResize();

	// Update the viewport transform to cover the client area.
	D3DViewportResource ScreenViewport;
	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = 800.0f;
	ScreenViewport.Height = 600.0f;
	ScreenViewport.MinDepth = 0.0f;
	ScreenViewport.MaxDepth = 1.0f;

	RenderInterface->SetViewport(ScreenViewport, SwapChain.get());

	// 리소스가 있을때 사용되는거라 일단 주석처리
// 	BuildRootSignature();
// 	BuildShadersAndInputLayout();
// 	BuildShapeGeometry();
// 	BuildMaterials();
// 	BuildRenderItems();
// 	BuildFrameResources();
// 	BuildPSOs();

	return true;
}

void D3D12Renderer::Update(GameTimer& gt)
{
// 	OnKeyboardInput(gt);
// 	UpdateCamera(gt);
// 
// 	// Cycle through the circular frame resource array.
// 	CurFrameResourceIndex = (CurFrameResourceIndex + 1) % gNumFrameResources;
// 	CurFrameResource = mFrameResources[CurFrameResourceIndex].get();
// 
// 	// Has the GPU finished processing the commands of the current frame resource?
// 	// If not, wait until the GPU has completed commands up to this fence point.
// 	if (CurFrameResource->Fence != 0 && Fence->GetCompletedValue() < CurFrameResource->Fence)
// 	{
// 		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
// 		ThrowIfFailed(Fence->SetEventOnCompletion(CurFrameResource->Fence, eventHandle));
// 		WaitForSingleObject(eventHandle, INFINITE);
// 		CloseHandle(eventHandle);
// 	}
// 
// 	AnimateMaterials(gt);
// 	UpdateObjectCBs(gt);
// 	UpdateMaterialCBs(gt);
// 	UpdateMainPassCB(gt);

//	UpdateObjectManager->UpdateTransform(gt);
}

// 1 드로우 = 1 프레임
void D3D12Renderer::Render(GameTimer& gt)
{
	// Ready to draw
	RenderInterface->ResetCommandList();

	RenderInterface->UpdateViewport(SwapChain.get());
	RenderInterface->ReadyToRenderTarget(GetCurrentBackBuffer(), GetCurrentBackBufferView(), GetDepthStencilBufferView());

// 	// --test
//  	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvHeap.Get() };
//  	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//  	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	Renderer->RenderScreenView(RenderInterface->GetCommnadList());

//  	ID3D12Resource* PassConstBuffer = CurFrameResource->PassConstBuffer->Resource();
//  	CommandList->SetGraphicsRootConstantBufferView(2, PassConstBuffer->GetGPUVirtualAddress()); // 임시 주석처리

	// End draw
	RenderInterface->FinishToRenderTarget(GetCurrentBackBuffer());

	// Finish to recode resource
	RenderInterface->ExecuteCommandList();
	SwapChain->SwapBackBufferToFrontBuffer();
	RenderInterface->FlushCommandQueue();
}

// void D3D12Renderer::UpdateCamera(const GameTimer& gt)
// {
// 	// Convert Spherical to Cartesian coordinates.
// 	mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta);
// 	mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta);
// 	mEyePos.y = mRadius * cosf(mPhi);
// 
// 	// Build the view matrix.
// 	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
// 	XMVECTOR target = XMVectorZero();
// 	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
// 
// 	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
// 	XMStoreFloat4x4(&mView, view);
// }

void D3D12Renderer::OnResize()
{
	RenderInterface->ResetCommandList();

	SwapChain->OnResize();

	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	DepthStencilBuffer = new D3D12DepthStencilResource();

	RenderInterface->ExecuteCommandList();
	RenderInterface->FlushCommandQueue();
}

void D3D12Renderer::AddScene(Scene* InScene)
{
	assert(InScene || Renderer);
	Renderer->AddScene(InScene);
}

void D3D12Renderer::SetCurrentScene(int InIndex)
{
	assert(Renderer);
	Renderer->SetCurrentScene(InIndex);
}

D3D12Resource* D3D12Renderer::GetCurrentBackBuffer() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBuffer();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Renderer::GetCurrentBackBufferView() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBufferView();
}
void D3D12Renderer::SwapBackBufferToFrontBuffer()
{
	assert(SwapChain);
	SwapChain->SwapBackBufferToFrontBuffer();
}

DXGI_FORMAT D3D12Renderer::GetBackBufferFormat() const
{
	assert(SwapChain);
	return SwapChain->GetBackBufferFormat();
}

DXGI_FORMAT D3D12Renderer::GetDepthStencilFormat() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilFormat();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Renderer::GetDepthStencilBufferView() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilView();
}