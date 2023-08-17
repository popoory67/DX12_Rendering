#include "SceneRendering.h"
#include "Mesh.h"
#include "Scene.h"
#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "D3D12RenderInterface.h"

SceneRenderer::SceneRenderer(std::shared_ptr<RHI> InRenderInterface)
{
	RenderInterface = std::static_pointer_cast<D3D12RHI>(InRenderInterface);
}

bool SceneRenderer::Initialize()
{
	return true;
}

void SceneRenderer::BeginRender()
{
	RenderInterface->ResetCommandList();
}

void SceneRenderer::Render(RHICommandList& InCommandList)
{
	RenderGraph::Get().Execute();
}

void SceneRenderer::EndRender()
{
	RenderInterface->FlushCommandQueue();
}