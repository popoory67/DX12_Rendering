#include "stdafx.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"

D3D12CommandList::D3D12CommandList(D3D12Device* InD3D12Device)
{
	assert(InD3D12Device || InD3D12Device->GetDevice());

	auto pDevice = InD3D12Device->GetDevice().Get();

	assert(pDevice);

	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandListAllocator.GetAddressOf())));
	ThrowIfFailed(pDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandListAllocator.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(CommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	CommandList->Close();
}

D3D12CommandListExecutor::D3D12CommandListExecutor(D3D12Device* InD3D12Device)
{
	assert(InD3D12Device || InD3D12Device->GetDevice());

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(InD3D12Device->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue)));

	Fence = new D3D12Fence(InD3D12Device);
}

void D3D12CommandListExecutor::Execute(D3D12CommandList* InCommandList)
{
	if (InCommandList)
	{
		ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = InCommandList->Get();

		assert(d3d12CommandList);
		ThrowIfFailed(d3d12CommandList->Close());

		ID3D12CommandList* cmdsLists[] = { d3d12CommandList.Get() };
		CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}
}

void D3D12CommandListExecutor::FlushCommands()
{
	assert(Fence);

	// Advance the fence value to mark commands up to this fence point.
	CurrentFenceCount++;

	Signal();
// 	// Add an instruction to the command queue to set a new fence point.  Because we 
// 	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
// 	// processing all the commands prior to this Signal().
// 	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFenceCount));

	// Wait until the GPU has completed commands up to this fence point.
	if (Fence->Get()->GetCompletedValue() < CurrentFenceCount)
	{
		Fence->OnEventCompletion(CurrentFenceCount);
// 		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
// 
// 		// Fire event when GPU hits current fence.  
// 		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFenceCount, eventHandle));
// 
// 		// Wait until the GPU hits current fence event is fired.
// 		WaitForSingleObject(eventHandle, INFINITE);
// 		CloseHandle(eventHandle);
	}
}

void D3D12CommandListExecutor::Signal()
{
	assert(Fence);

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(CommandQueue->Signal(Fence->Get(), CurrentFenceCount));
}

void D3D12CommandList::Reset()
{
	// GPU가 command list의 명령을 모두 처리한 후에 리셋
 	ThrowIfFailed(CommandListAllocator->Reset());
 
 	// 이전에 Excute하여 Queue에 command list를 추가했으니 list를 재설정해도 된다.
 	ThrowIfFailed(CommandList->Reset(CommandListAllocator.Get(),/* OpaquePipelineStateObject.Get()*/ nullptr));
}

void D3D12CommandList::DrawRenderItems()
{
	// test
// 	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvHeap.Get() };
// 	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
// 
// 	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
// 	UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
// 
// 	auto ObjectConstBuffer = ObjectBuffer->Get()->Resource();
// 	auto matCB = MaterialBuffer->Get()->Resource();
// 
// 	// For each render item...
// 	for (size_t i = 0; i < RenderItems.size(); ++i)
// 	{
// 		auto ri = RenderItems[i];
// 
// 		CommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
// 		CommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
// 		CommandList->IASetPrimitiveTopology(ri->PrimitiveType);
// 
// 		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
// 		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
// 
// 		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = ObjectConstBuffer->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
// 		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;
// 
// 		CommandList->SetGraphicsRootDescriptorTable(0, tex);
// 		CommandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
// 		CommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
// 
// 		CommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
// 	}
}

// 
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
// 
// void D3DApp::UpdateObjectCBs(const GameTimer& gt)
// {
// 	for (auto& e : AllRitems)
// 	{
// 		// Only update the cbuffer data if the constants have changed.  
// 		// This needs to be tracked per frame resource.
// 		if (e->NumFramesDirty > 0)
// 		{
// 			XMMATRIX world = XMLoadFloat4x4(&e->World);
// 			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);
// 
// 			ObjectConstants objConstants;
// 			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
// 			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
// 
// 			ObjectBuffer.Get()->CopyData(e->ObjCBIndex, objConstants);
// 
// 			// Next FrameResource need to be updated too.
// 			e->NumFramesDirty--;
// 		}
// 	}
// }
// 
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