#include "stdafx.h"
#include "D3DApp.h"
#include <combaseapi.h>
#include "GeometryGenerator.h"
#include "DDSTextureLoader.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12SwapChain.h"
#include "D3D12Commands.h"
#include "D3D12PipelineState.h"

D3DApp* D3DApp::Instance = nullptr;

D3DApp::D3DApp()
{
	// Only one D3DApp can be constructed.
	assert(Instance == nullptr);
	Instance = this;
}

D3DApp::~D3DApp()
{
}

D3DApp& D3DApp::GetInstance()
{
	if (!Instance)
		Instance = new D3DApp();

	return *Instance;
}

bool D3DApp::Initialize(HWND InWindowHandle)
{
	D3D12Device* pDevice = new D3D12Device(InWindowHandle);
	assert(pDevice);

	CommandList = new D3D12CommandList(pDevice);
	assert(CommandList);

	RenderInterface = new D3D12RenderInterface(pDevice, CommandList);
	assert(RenderInterface);

 	RenderInterface->OnResize(CommandList);

	// Update the viewport transform to cover the client area.
	D3DViewportResource ScreenViewport;
	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = 800.0f;
	ScreenViewport.Height = 600.0f;
	ScreenViewport.MinDepth = 0.0f;
	ScreenViewport.MaxDepth = 1.0f;

	RenderInterface->SetViewport(ScreenViewport);

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

void D3DApp::Update(const GameTimer& gt)
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
}

// 1 드로우 = 1 프레임
void D3DApp::Draw(const GameTimer& gt)
{
	// Ready to draw
	CommandList->Reset();

	RenderInterface->UpdateViewport(CommandList);
	RenderInterface->ReadyToRenderTarget(CommandList);

	// --test
//  	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvHeap.Get() };
//  	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//  
//  	CommandList->SetGraphicsRootSignature(RootSignature.Get());
//  
//  	ID3D12Resource* PassConstBuffer = CurFrameResource->PassConstBuffer->Resource();
//  	CommandList->SetGraphicsRootConstantBufferView(2, PassConstBuffer->GetGPUVirtualAddress()); // 임시 주석처리
// 
// 	DrawRenderItems(CommandList.Get(), OpaqueRitems);

	// End draw
	RenderInterface->FinishToRenderTarget(CommandList);

	// Finish to recode resource
	RenderInterface->ExecuteCommandList(CommandList);
	RenderInterface->SwapBackBufferToFrontBuffer();
	RenderInterface->FlushCommandQueue();
}

// void D3DApp::UpdateCamera(const GameTimer& gt)
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