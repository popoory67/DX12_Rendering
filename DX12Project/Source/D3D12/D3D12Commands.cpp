#include "stdafx.h"
#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
{
	assert(InDevice);

	InDevice->CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, CommandListAllocator, CommandList);

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	CommandList->Close();
}

void D3D12CommandList::Reset()
{
	// GPU�� command list�� ����� ��� ó���� �Ŀ� ����
	ThrowIfFailed(CommandListAllocator->Reset());

	// ������ Excute�Ͽ� Queue�� command list�� �߰������� list�� �缳���ص� �ȴ�.
	ThrowIfFailed(CommandList->Reset(CommandListAllocator.Get(),/* OpaquePipelineStateObject.Get()*/ nullptr));
}

ID3D12CommandList* D3D12CommandList::GetCommandLists()
{
	assert(CommandList || CommandList.Get());
	ThrowIfFailed(CommandList->Close());

	return CommandList.Get();
}

void D3D12CommandList::SetResourceTransition(D3D12Resource* InResource, D3D12_RESOURCE_STATES InPrevState, D3D12_RESOURCE_STATES InNextState)
{
	assert(CommandList);
	assert(InResource);

	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(InResource->GetInterface(), InPrevState, InNextState));
}

void D3D12CommandList::ResourceBarrier(D3D12Resource* InResource, D3D12_RESOURCE_STATES InFrom, D3D12_RESOURCE_STATES InTo)
{
	assert(CommandList);
	assert(InResource);

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(InResource->GetInterface(), InFrom, InTo));
}

void D3D12CommandList::ClearRenderTargetView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, XMVECTORF32 InBackColor, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	assert(CommandList);
	CommandList->ClearRenderTargetView(InDescriptorHandle.value(), InBackColor, InNumRects, InRect);
}

void D3D12CommandList::ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	assert(CommandList);
	CommandList->ClearDepthStencilView(InDescriptorHandle.value(), ClearFlags, InDepthValue, InStencil, InNumRects, InRect);
}

void D3D12CommandList::SetRenderTargets(UINT InNumRenderTargetDescriptors, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InRenderTargetDescriptorHandle, bool InSingleHandleToDescriptorRange, std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDepthStencilDescriptorHandle)
{
	assert(CommandList);
	CommandList->OMSetRenderTargets(InNumRenderTargetDescriptors, &InRenderTargetDescriptorHandle.value(), InSingleHandleToDescriptorRange, &InDepthStencilDescriptorHandle.value());
}

void D3D12CommandList::SetPipelineState(D3D12PipelineState* InPipelineState) const
{
	assert(CommandList);
	assert(InPipelineState);

	CommandList->SetPipelineState(InPipelineState->GetInterface());
}

void D3D12CommandList::SetVertexBuffers(D3D12VertexBufferCache& InVertexBufferCache) const
{
	assert(CommandList);

	// TODO : �̰� ��� ȣ�������� => � ���� �׸����� �غ��ϴ� ������ ȣ��, �� ������ �����������Ҷ� = �����ϴ� ����� ������ �ְ�(�����) �׸��°� �������� �˾Ƽ�
	// D3D12VertexBuffer* InVertexBuffer, uint32_t StreamIndex, uint32_t InStride, uint32_t InOffset
//	StateCache.SetStreamSource(InVertexBuffer, StreamIndex, InStride, InOffset);
	
	CommandList->IASetVertexBuffers(0, InVertexBufferCache.MaxVertexIndex + 1, InVertexBufferCache.CurrentVertexBufferView);
}

void D3D12CommandList::SetIndexBuffer(std::optional<D3D12_INDEX_BUFFER_VIEW> InView/* = {}*/) const
{
	assert(CommandList);
	CommandList->IASetIndexBuffer(&InView.value());
}

void D3D12CommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY InPrimitiveTopology) const
{
	assert(CommandList);
	CommandList->IASetPrimitiveTopology(InPrimitiveTopology);
}

void D3D12CommandList::DrawIndexedInstanced(UINT InIndexCountPerInstance, UINT InInstanceCount, UINT InStartIndexLocation, INT InBaseVertexLocation, UINT InStartInstanceLocation) const
{
	assert(CommandList);
	CommandList->DrawIndexedInstanced(InIndexCountPerInstance, InInstanceCount, InStartIndexLocation, InBaseVertexLocation, InStartInstanceLocation);
}

void D3D12CommandList::AddDescriptorHeap(D3D12Descriptor* InDescriptor)
{
	assert(CommandList);
	assert(InDescriptor);

	Heaps.emplace_back(InDescriptor->Get());
}

void D3D12CommandList::ExecuteHeaps()
{
	assert(CommandList);

	if (!Heaps.empty())
		CommandList->SetDescriptorHeaps((UINT)Heaps.size(), &(*Heaps.begin()));
}

void D3D12CommandList::FlushHeaps()
{
	Heaps.clear();
}

void D3D12CommandList::SetRootSignature(D3D12RootSignature* InRootSignature)
{
	assert(CommandList);
	assert(InRootSignature);

	CommandList->SetGraphicsRootSignature(InRootSignature->GetInterface());
}

void D3D12CommandList::BindTable(RenderType InRenderType, CD3DX12_GPU_DESCRIPTOR_HANDLE InDescriptorHandle)
{
	assert(CommandList);
/*	InDescriptorHandle.Offset(mSkyTexHeapIndex, mCbvSrvUavDescriptorSize);*/
	CommandList->SetGraphicsRootDescriptorTable((UINT)InRenderType, InDescriptorHandle);
}

void D3D12CommandList::BindConstBuffer(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses)
{
	assert(CommandList);

	for (D3D12_GPU_VIRTUAL_ADDRESS address : InAddresses)
		CommandList->SetGraphicsRootConstantBufferView((UINT)InRenderType, address);
}

void D3D12CommandList::BindConstBuffer(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress)
{
	assert(CommandList);
	CommandList->SetGraphicsRootConstantBufferView((UINT)InRenderType, InAddress);
}

void D3D12CommandList::BindShaderResource(RenderType InRenderType, std::vector<D3D12_GPU_VIRTUAL_ADDRESS>& InAddresses)
{
	assert(CommandList);

	for (D3D12_GPU_VIRTUAL_ADDRESS address : InAddresses)
		CommandList->SetGraphicsRootShaderResourceView((UINT)InRenderType, address);
}

void D3D12CommandList::BindShaderResource(RenderType InRenderType, D3D12_GPU_VIRTUAL_ADDRESS& InAddress)
{
	assert(CommandList);
	CommandList->SetGraphicsRootShaderResourceView((UINT)InRenderType, InAddress);
}

D3D12CommandListExecutor::D3D12CommandListExecutor(D3D12Device* InDevice)
{
	assert(InDevice);

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	InDevice->CreateCommandQueue(queueDesc, CommandQueue);

	Fence = new D3D12Fence(InDevice);
}

D3D12CommandListExecutor::~D3D12CommandListExecutor()
{
}

void D3D12CommandListExecutor::Execute(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// �̺κ��� �����ؾ���
	ID3D12CommandList* cmdsLists[] = { InCommandList->GetCommandLists() };
	CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void D3D12CommandListExecutor::FlushCommands()
{
	assert(Fence);

	// Advance the fence value to mark commands up to this fence point.
	CurrentFenceCount++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(CommandQueue->Signal(Fence->Get(), CurrentFenceCount));

	// Wait until the GPU has completed commands up to this fence point.
	if (Fence->Get()->GetCompletedValue() < CurrentFenceCount)
	{
		Fence->OnEventCompletion(CurrentFenceCount);
	}
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