#include "RenderPass.h"
#include "ThreadBase.h"
#include "CommandList.h"

RenderPass::RenderPass()
    : Type(RenderPassType::None)
{

}

RenderGraph::RenderGraph()
{

}

RenderGraph& RenderGraph::Get()
{
    static RenderGraph graphInterface;
    return graphInterface;
}

void RenderGraph::AddTask(RenderPass*&& InRenderPass)
{
    // TODO
    // Render pass has a priority, and we need to compare it.

    TaskInternal = std::move(InRenderPass);

    //TaskGraphSystem::Get().AddTask(this, ThreadType::Render);
}