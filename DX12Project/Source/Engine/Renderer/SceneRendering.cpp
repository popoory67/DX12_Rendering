#include "stdafx.h"
#include "SceneRendering.h"
#include "Mesh.h"
#include "Scene.h"
#include "ObjectCommand.h"
#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "D3D12RenderInterface.h"
#include "D3D12Commands.h" // it must be removed

// thread test
void RenderingThread::ThreadProc()
{
    if (Action->Init() == true)
    {
        Action->Run();
    }
}

bool SceneRenderer::Init()
{
	return true;
}

void SceneRenderer::Run()
{

}

void SceneRenderer::Stop()
{

}

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
// 	BuildRootSignature();
// 	BuildShadersAndInputLayout();
// 	BuildShapeGeometry();
// 	BuildMaterials();
// 	BuildRenderItems();
// 	BuildFrameResources();
// 	BuildPSOs();

	return true;
}

void SceneRenderer::BeginRender()
{
	RenderInterface->ResetCommandList();
}

void SceneRenderer::Render(RHICommandList& InCommandList)
{
	// TODO
	// Process render passes
}

void SceneRenderer::EndRender()
{
	RenderInterface->FlushCommandQueue();
}

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
