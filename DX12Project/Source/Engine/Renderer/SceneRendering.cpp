#include "SceneRendering.h"
#include "RenderPass.h"
#include "CommandContext.h"

bool SceneRenderer::Initialize()
{
	return true;
}

void SceneRenderer::BeginRender()
{
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