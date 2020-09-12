#include "stdafx.h"
#include "SceneRendering.h"
#include "D3D12RenderInterface.h"
#include "D3D12RootSignature.h"
#include "Mesh.h"
#include "Scene.h"
#include "ObjectCommand.h"

void SceneRenderer::RenderScreenView(D3D12CommandList& InCommandList)
{
	//assert(RootSignature);

	InCommandList.ExecuteHeaps();
	//InCommandList->SetRootSignature(RootSignature);

	RenderPrimitives(InCommandList);
	// �� ���̿� material, shadow ���� �־���ϰ�
	// command list���� render �������� root signature ó���� �� �� ī��Ʈ�� ���������� �����ؾ���
	RenderMaterials(InCommandList);

	InCommandList.FlushHeaps();
}

void SceneRenderer::RenderPrimitives(D3D12CommandList& InCommandList)
{
	// TODO : Component ����Ʈ�� �������°� �ƴ϶�, component �ȿ� �ִ� mirror data�� �����ͼ� ������
	// Component�� Render�� �и��� �Ǿ��־�� �Ѵ�.
	// Component���� Draw, Render �Լ� ȣ�� => �ݵ�� �� �����带 �и��ؼ� ó��
	for (auto pCommand : Commands)
	{
		pCommand->ExecuteAndDestruct(InCommandList);
	}

	
	//D3D12UploadResource<PrimitiveComponent>* pConstBuffer = new D3D12UploadResource<PrimitiveComponent>(true);
	//RenderInterface::GetInterface()->CreateUploadResource<PrimitiveComponent>(pConstBuffer, 0);

	std::vector<PrimitiveComponent*> OutComponents;
	GetCurrentScene()->GetComponents<PrimitiveComponent>(OutComponents);

	unsigned int index = 0;
	for (auto pPrimitive : OutComponents)
	{
		if (pPrimitive)
		{
			InCommandList.SetVertexBuffers();
			InCommandList->SetIndexBuffer();
			InCommandList->SetPrimitiveTopology(pPrimitive->PrimitiveType);

			InCommandList->DrawIndexedInstanced(pPrimitive->GetIndexCount(), 1, pPrimitive->GetStartIndexLocation(), pPrimitive->GetBaseVertexLocation(), 0);

			D3D12_GPU_VIRTUAL_ADDRESS address = pConstBuffer->GetResource()->GetGPUVirtualAddress() + index * pPrimitive->GetBindingSize();
			InCommandList->BindConstBuffer(RenderType::Primitive, address);

			++index;
		}
	}
}

void SceneRenderer::RenderMaterials(D3D12CommandList& InCommandList)
{
 	//UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
 
 	//auto matCB = MaterialConstBuffer->Resource();
 	//D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress();
 
 	//for (auto it : MaterialItems)
 	//{
 	//	const Material* pMaterial = it.second;
 	//	if (pMaterial)
 	//	{
 	//		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
 	//		tex.Offset(pMaterial->GetDiffuseSrvHeapIndex(), CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
 
 	//		InCommandList->SetGraphicsRootDescriptorTable(0, tex);
 
 	//		D3D12_GPU_VIRTUAL_ADDRESS address = matCBAddress + pMaterial->GetIndex() * matCBByteSize;
 
 	//		InCommandList->SetGraphicsRootConstantBufferView(3, address);
 	//	}
 	//}
}

std::shared_ptr<class Scene> SceneRenderer::GetCurrentScene()
{
	return CurrentScene;
}

void SceneRenderer::AddScene(Scene* InScene)
{
	assert(InScene);

	auto it = SceneList.find(InScene->GetSceneId());
	if (it == SceneList.cend())
	{
		InScene->SetSceneId(IndexCount);
		SceneList.emplace(std::make_pair(IndexCount, InScene));

		++IndexCount;
	}
}

void SceneRenderer::SetCurrentScene(int InIndex)
{
	auto it = SceneList.find(InIndex);
	if (it != SceneList.cend())
	{
		if (InIndex >= 0)
		{
			CurrentSceneIndex = InIndex;
			CurrentScene = it->second;
		}
	}
	else
	{
		assert("There isn't such scene");
	}
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