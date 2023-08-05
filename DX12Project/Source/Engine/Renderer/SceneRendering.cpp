#include "SceneRendering.h"
#include "Mesh.h"
#include "Scene.h"
#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "D3D12RenderInterface.h"
#include "D3D12Commands.h" // it must be removed


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