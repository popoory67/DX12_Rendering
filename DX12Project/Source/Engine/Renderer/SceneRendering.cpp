#include "SceneRendering.h"
#include "RenderPass.h"
#include "CommandContext.h"

bool SceneRenderer::Initialize()
{
	return true;
}

void SceneRenderer::BeginRender()
{
	// Set root signatures on a command list

    RenderGraph::Get().Execute();
}

void SceneRenderer::Render(RHICommandContext& InContext)
{	
	// Set constant buffers

	InContext.ExecuteCommands();
}

void SceneRenderer::EndRender()
{
	//GRHI->FlushCommandQueue();
}