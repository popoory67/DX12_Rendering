#include "stdafx.h"
#include "SceneRendering.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12RootSignature.h"
#include "Entity.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"


SceneRenderer::SceneRenderer()
{

}

SceneRenderer::~SceneRenderer()
{

}

void SceneRenderer::Initialize(D3D12Device* InD3D12Device)
{
	ObjectConstBuffer = std::make_unique<D3D12UploadResource<Primitive>>(InD3D12Device, PrimitiveItems.size(), true);
}

void SceneRenderer::RenderScreenView(D3D12CommandList* InCommandList)
{
	assert(InCommandList);
	assert(RootSignature);

	InCommandList->ExecuteHeaps();
	InCommandList->SetRootSignature(RootSignature);

	RenderPrimitives(InCommandList);
	// 이 사이에 material, shadow 등이 있어야하고
	// command list에서 render 과정에서 root signature 처리를 할 때 카운트를 내부적으로 관리해야함
	RenderMaterials(InCommandList);

	InCommandList->FlushHeaps();
}

void SceneRenderer::RenderPrimitives(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(Transform));

	// scenelist
	for (auto it : PrimitiveItems)
	{
		const Primitive* pPrimitive = it.second;
		if (pPrimitive)
		{
			InCommandList->SetVertexBuffers(0, 1, pPrimitive->VertexBufferView());
			InCommandList->SetIndexBuffer(pPrimitive->IndexBufferView());
			InCommandList->SetPrimitiveTopology(pPrimitive->PrimitiveType);

			InCommandList->DrawIndexedInstanced(pPrimitive->GetIndexCount(), 1, pPrimitive->GetStartIndexLocation(), pPrimitive->GetBaseVertexLocation(), 0);
		}

		D3D12_GPU_VIRTUAL_ADDRESS address = ObjectConstBuffer->Resource()->GetGPUVirtualAddress() + pPrimitive->GetIndex() * objCBByteSize;

		InCommandList->BindConstBuffer(RenderType::Primitive, address);
	}
}

void SceneRenderer::RenderMaterials(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

// 	UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
// 
// 	auto matCB = MaterialConstBuffer->Resource();
// 	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress();
// 
// 	for (auto it : MaterialItems)
// 	{
// 		const Material* pMaterial = it.second;
// 		if (pMaterial)
// 		{
// 			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
// 			tex.Offset(pMaterial->GetDiffuseSrvHeapIndex(), CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
// 
// 			InCommandList->SetGraphicsRootDescriptorTable(0, tex);
// 
// 			D3D12_GPU_VIRTUAL_ADDRESS address = matCBAddress + pMaterial->GetIndex() * matCBByteSize;
// 
// 			InCommandList->SetGraphicsRootConstantBufferView(3, address);
// 		}
// 	}
}

void SceneRenderer::SetCurrentScene(Scene * InScene)
{
	assert(InScene);
	CurrentScene = InScene;
}

void SceneRenderer::SceneRendering()
{
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