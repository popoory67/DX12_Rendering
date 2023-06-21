#include "stdafx.h"
#include "SceneRendering.h"
#include "Mesh.h"
#include "Scene.h"
#include "ObjectCommand.h"
#include "PrimitiveComponent.h"

SceneRenderer::SceneRenderer(std::shared_ptr<RHI> InRenderInterface)
{
	RenderInterface = std::static_pointer_cast<D3D12RHI>(InRenderInterface);
}

std::shared_ptr<Scene> SceneRenderer::GetCurrentScene()
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
		assert("There isn't any scene");
	}
}

bool SceneRenderer::Initialize()
{
	//RenderInterface->ResetCommandList();
	//RenderInterface->ExecuteCommandList();
	//RenderInterface->FlushCommandQueue();

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

void SceneRenderer::Update(GameTimer& gt)
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

void SceneRenderer::BeginRender()
{
	RenderInterface->ResetCommandList();
}

void SceneRenderer::Render(GameTimer& gt)
{

}

void SceneRenderer::EndRender()
{
	RenderInterface->FlushCommandQueue();
}

// void SceneRenderer::UpdateCamera(const GameTimer& gt)
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

void SceneRenderer::RenderPrimitives(D3D12CommandList& InCommandList)
{
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
			//InCommandList.SetVertexBuffers();
			InCommandList.SetIndexBuffer();
			InCommandList.SetPrimitiveTopology(pPrimitive->PrimitiveType);

			//InCommandList.DrawIndexedInstanced(pPrimitive->GetIndexCount(), 1, pPrimitive->GetStartIndexLocation(), pPrimitive->GetBaseVertexLocation(), 0);

			//D3D12_GPU_VIRTUAL_ADDRESS address = pConstBuffer->GetResource()->GetGPUVirtualAddress() + index * pPrimitive->GetBindingSize();
			//InCommandList.BindConstBuffer(RenderType::Primitive, address);

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
