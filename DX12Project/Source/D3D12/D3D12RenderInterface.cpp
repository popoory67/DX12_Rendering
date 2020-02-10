#include "stdafx.h"
//#include <d3d12.h>
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"

// test
#include "Mesh.h"

D3D12RenderInterface::D3D12RenderInterface(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	assert(InDevice);
	assert(InCommandList);

	Device = InDevice;

	CmdListExecutor = new D3D12CommandListExecutor(Device);

	SwapChain = new D3D12SwapChain(Device);
	if (SwapChain)
	{
		SwapChain->Create(Device, CmdListExecutor);
	}

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	// test
	D3D12PipelineState* pPipelineState = new D3D12PipelineState(InDevice, GetBackBufferFormat(), GetDepthStencilFormat());
	if (pPipelineState)
	{
		PipelineStates.emplace_back(pPipelineState);
	}
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

DXGI_FORMAT D3D12RenderInterface::GetBackBufferFormat() const
{ 
	assert(SwapChain); 
	return SwapChain->GetBackBufferFormat(); 
}

DXGI_FORMAT D3D12RenderInterface::GetDepthStencilFormat() const
{ 
	assert(DepthStencilBuffer); 
	return DepthStencilBuffer->GetDepthStencilFormat(); 
}

void D3D12RenderInterface::OnResize(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	InCommandList->Reset();

	SwapChain->OnResize(Device);

	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	ExecuteCommandList(InCommandList);
	FlushCommandQueue();
}

void D3D12RenderInterface::ExecuteCommandList(D3D12CommandList* InCommandList)
{
	for (D3D12PipelineState* pPso : PipelineStates)
	{
		InCommandList->SetPipelineState(pPso);
		
		// render
		DrawRenderItems(InCommandList);
	}

	CmdListExecutor->Execute(InCommandList);
}

void D3D12RenderInterface::FlushCommandQueue() const
{
	CmdListExecutor->FlushCommands();
}

void D3D12RenderInterface::UpdateViewport(class D3D12CommandList* InCommandList)
{
	if (InCommandList && SwapChain)
	{
		InCommandList->Get()->RSSetViewports(1, &SwapChain->GetViewport());
		InCommandList->Get()->RSSetScissorRects(1, &SwapChain->GetRect());
	}
}
void D3D12RenderInterface::SetViewport(class D3DViewportResource& InViewResource)
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

void D3D12RenderInterface::ReadyToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the back buffer and depth buffer.
	InCommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0);
	InCommandList->ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

	// Specify the buffers we are going to render to.
	InCommandList->SetRenderTargets(1, GetCurrentBackBufferView(), true, GetDepthStencilBufferView());
}

void D3D12RenderInterface::FinishToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(SwapChain->GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

D3D12Resource* D3D12RenderInterface::GetCurrentBackBuffer() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBuffer();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetCurrentBackBufferView() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBufferView();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetDepthStencilBufferView() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilView();
}

void D3D12RenderInterface::SwapBackBufferToFrontBuffer()
{
	assert(SwapChain);
	SwapChain->SwapBackBufferToFrontBuffer();
}

// 여기부터 그리기 구현
void D3D12RenderInterface::DrawRenderItems(D3D12CommandList* InCommandList)
{
	ComPtr<ID3D12GraphicsCommandList> d3d12CommantList = InCommandList->Get();

	// For each render item...
	for (size_t i = 0; i < OpaqueRitems.size(); ++i)
	{
		auto ri = OpaqueRitems[i];

		d3d12CommantList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		d3d12CommantList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		d3d12CommantList->IASetPrimitiveTopology(ri->PrimitiveType);

		// 텍스쳐 옮겨야함
		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
		
		// 이거 옮겨야함
		{
			UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
			UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

			//auto objectCB = CurFrameResource->ObjectConstBuffer->Resource();
			//auto matCB = CurFrameResource->MaterialConstBuffer->Resource();

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
			D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;

			d3d12CommantList->SetGraphicsRootDescriptorTable(0, tex);
			d3d12CommantList->SetGraphicsRootConstantBufferView(1, objCBAddress);
			d3d12CommantList->SetGraphicsRootConstantBufferView(3, matCBAddress);
		}

		d3d12CommantList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void D3D12RenderInterface::RenderPrimitives(D3D12CommandList* InCommandList)
{
	for (auto it : PrimitiveItems)
	{
		Primitive* pPrimitive = it.second;
		if (pPrimitive)
		{
			InCommandList->SetVertexBuffers(0, 1, pPrimitive->VertexBufferView());
			InCommandList->SetIndexBuffer(pPrimitive->IndexBufferView());
			InCommandList->SetPrimitiveTopology(pPrimitive->GetTopology());

			InCommandList->DrawIndexedInstanced(pPrimitive->GetIndexCount(), 1, pPrimitive->GetStartIndexLocation(), pPrimitive->GetBaseVertexLocation(), 0);
		}
	}
}

void D3D12RenderInterface::RenderMaterials(class D3D12CommandList* InCommandList)
{
	for (auto it : MaterialItems)
	{
		Material* pMaterial = it.second;
		if (pMaterial)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
			tex.Offset(pMaterial->GetDiffuseSrvHeapIndex(), CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
		}
	}
}

// void D3DApp::UpdateMaterialCBs(const GameTimer& gt)
// {
// 	for (auto& e : mMaterials)
// 	{
// 		// Only update the cbuffer data if the constants have changed.  If the cbuffer
// 		// data changes, it needs to be updated for each FrameResource.
// 		Material* mat = e.second.get();
// 		if (mat->NumFramesDirty > 0)
// 		{
// 			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);
// 
// 			MaterialConstants matConstants;
// 			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
// 			matConstants.FresnelR0 = mat->FresnelR0;
// 			matConstants.Roughness = mat->Roughness;
// 			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));
// 
// 			MaterialBuffer.Get()->CopyData(mat->MatCBIndex, matConstants);
// 
// 			// Next FrameResource need to be updated too.
// 			mat->NumFramesDirty--;
// 		}
// 	}
// }

// void D3DApp::UpdateMainPassCB(const GameTimer& gt)
// {
// 	XMMATRIX view = XMLoadFloat4x4(&mView);
// 	XMMATRIX proj = XMLoadFloat4x4(&mProj);
// 
// 	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
// 	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
// 	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
// 	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
// 
// 	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
// 	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
// 	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
// 	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
// 	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
// 	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
// 	mMainPassCB.EyePosW = mEyePos;
// 	mMainPassCB.RenderTargetSize = XMFLOAT2((float)ClientWidth, (float)ClientHeight);
// 	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / ClientWidth, 1.0f / ClientHeight);
// 	mMainPassCB.NearZ = 1.0f;
// 	mMainPassCB.FarZ = 1000.0f;
// 	mMainPassCB.TotalTime = gt.TotalTime();
// 	mMainPassCB.DeltaTime = gt.DeltaTime();
// 	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
// 	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
// 	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
// 	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
// 	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
// 	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
// 	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
// 
// 	auto currPassCB = CurFrameResource->PassConstBuffer.get();
// 	currPassCB->CopyData(0, mMainPassCB);
// }