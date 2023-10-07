#include "RenderPass.h"
#include "ThreadBase.h"
#include "CommandList.h"

RenderPass::RenderPass()
    : Type(RenderPassType::None)
{

}

RenderGraph& RenderGraph::Get()
{
    static RenderGraph graphInterface;
    return graphInterface;
}

void RenderGraph::AddTask(std::unique_ptr<RenderPass> InRenderPass)
{
    // TODO
    // Render pass has a priority, and we need to compare it.

    TaskInternal = std::move(InRenderPass);
    bCompleted = false;

    //TaskGraphSystem::Get().AddTask(this, ThreadType::Render);
}