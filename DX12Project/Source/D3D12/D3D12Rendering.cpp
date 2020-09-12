#include "stdafx.h"
#include "D3D12Rendering.h"
#include <combaseapi.h>
#include "GeometryGenerator.h"
#include "DDSTextureLoader.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Viewport.h"
#include "D3D12Commands.h"
#include "SceneRendering.h"
#include "Scene.h"

bool D3D12Renderer::Initialize()
{
	D3D12Device* pDevice = new D3D12Device();
	assert(pDevice);

	DeviceChild.reset(new D3D12DeviceChild(pDevice));
	assert(DeviceChild);

	Viewport.reset(new D3D12Viewport(DeviceChild.get()));
	assert(Viewport);

	//RenderInterface->ResetCommandList();
	//RenderInterface->ExecuteCommandList();
	//RenderInterface->FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	D3DViewportResource screenViewport;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = 800.0f;
	screenViewport.Height = 600.0f;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	Viewport->SetViewport(screenViewport);

	//Renderer.reset(new SceneRenderer(RenderInterface.get()));
	//assert(Renderer);

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

void D3D12Renderer::PreRender()
{
	assert(DeviceChild);

	auto pDevice = DeviceChild->GetParent();
	assert(pDevice);

	auto pCommandList = pDevice->GetCommandList();
	assert(pCommandList);

	// Ready to draw
	pCommandList->Reset();

	Viewport->UpdateViewport();
	Viewport->ReadyToRenderTarget();
}

// 1 드로우 = 1 프레임
void D3D12Renderer::Render(GameTimer& gt)
{
// 	// --test
//  	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvHeap.Get() };
//  	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//  	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	Renderer->RenderScreenView(pCommandList);

//  	ID3D12Resource* PassConstBuffer = CurFrameResource->PassConstBuffer->Resource();
//  	CommandList->SetGraphicsRootConstantBufferView(2, PassConstBuffer->GetGPUVirtualAddress()); // 임시 주석처리
}

void D3D12Renderer::PostRender()
{
	assert(DeviceChild);

	auto pDevice = DeviceChild->GetParent();
	assert(pDevice);

	auto pCommandList = pDevice->GetCommandList();
	assert(pCommandList);
	
	// End draw
	Viewport->FinishToRenderTarget();

	for (D3D12PipelineState* pPso : PipelineStates)
	{
		pCommandList->SetPipelineState(pPso);
	}

	pDevice->ExecuteCommands();

	Viewport->SwapBackBufferToFrontBuffer();

	pDevice->FlushCommandQueue();
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

	Viewport->OnResize();

	RenderInterface->ExecuteCommandList();
	RenderInterface->FlushCommandQueue();
}

//void D3D12Renderer::AddScene(Scene* InScene)
//{
//	assert(InScene || Renderer);
//	Renderer->AddScene(InScene);
//}
//
//void D3D12Renderer::SetCurrentScene(int InIndex)
//{
//	assert(Renderer);
//	Renderer->SetCurrentScene(InIndex);
//}