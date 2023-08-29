#include "SceneRendering.h"
#include "Mesh.h"
#include "Scene.h"
#include "PrimitiveComponent.h"
#include "CommandContext.h"
#include "RenderInterface.h"
#include "D3D12RenderInterface.h"

bool SceneRenderer::Initialize()
{
	return true;
}

void SceneRenderer::BeginRender()
{
	//GRHI->ResetCommandList();

    RenderGraph::Get().Execute();
}

void SceneRenderer::Render(RHICommandContext& InContext)
{
	InContext.ExecuteCommands();
}

void SceneRenderer::EndRender()
{
	//GRHI->FlushCommandQueue();
}