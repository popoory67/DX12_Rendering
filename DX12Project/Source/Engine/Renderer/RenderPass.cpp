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