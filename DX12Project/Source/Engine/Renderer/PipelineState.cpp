#include "PipelineState.h"

bool PSOLibrary::Init()
{
    return true;
}

void PSOLibrary::Run()
{
    // Check if PSO exists with RHI.

    // Call Rendering pipeline and process PSO to RHI.
}

void PSOLibrary::Stop()
{

}

std::weak_ptr<PipelineState> PSOLibrary::GetPSOCache(const GraphicsPipelineState::Key& InDesc)
{
    return {};
}

bool PSOLibrary::Load(size_t InVertexHash, size_t InFragmentHash)
{
    return true;
}

void PSOLibrary::Save(size_t InVertexHash, size_t InFragmentHash)
{

}

void PSOLibrary::CreateAndAddCache(const GraphicsPipelineState::Key& InDesc)
{
    std::shared_ptr<PipelineState> pipelineState = std::make_shared<PipelineState>();
    if (pipelineState)
    {
        PipelineStateCaches.emplace(InDesc, pipelineState);
    }
}

std::weak_ptr<PipelineState> PSOLibrary::FindCache(const GraphicsPipelineState::Key& InDesc)
{
    auto it = PipelineStateCaches.find(InDesc);
    if (it != PipelineStateCaches.cend())
    {
        return it->second;
    }
    return {};
}