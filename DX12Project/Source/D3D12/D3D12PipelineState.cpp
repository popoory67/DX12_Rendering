#include "D3D12PipelineState.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12Commands.h"
#include "D3D12RootSignature.h"
#include "D3DUtil.h"

D3D12PipelineState::D3D12PipelineState(D3D12Device* InDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc)
	: D3D12Api(InDevice)
{
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc = InDesc;

	ThrowIfFailed(GetDevice()->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState)));
}

D3D12PipelineState::~D3D12PipelineState()
{
	PipelineState.Reset();
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC& D3D12PipelineState::GetDesc() const
{
	return PipelineStateDesc;
}

D3D12PipelineStateCache::D3D12PipelineStateCache(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{
	DescriptorCache = std::make_shared<D3D12DescriptorCache>(InDevice);
}

D3D12PipelineStateCache::~D3D12PipelineStateCache()
{
    SafeDelete(StateCache.RootSignature);
    SafeDelete(StateCache.DepthStencil);

    //for (int i = 0; i < _countof(StateCache.RenderTargets); ++i)
    //{
    //    SafeDelete(StateCache.RenderTargets[i]);
    //}
}

void D3D12PipelineStateCache::IssueCachedResources(D3D12CommandList& InCommandList)
{
	InCommandList->RSSetViewports(1, &StateCache.Viewport);
	InCommandList->RSSetScissorRects(1, &StateCache.ScissorRect);

	DescriptorCache->SetRenderTargets(InCommandList, StateCache.RenderTargets, StateCache.NumActivatedRenderTargets, StateCache.DepthStencil);

    DescriptorCache->SetVertexBuffers(InCommandList, StateCache.VertexBufferCache);
    DescriptorCache->SetIndexBuffers(InCommandList, StateCache.IndexBufferCache);
}

void D3D12PipelineStateCache::SetViewport(const D3D12_VIEWPORT& InViewport, const D3D12_RECT& InRect)
{
	StateCache.Viewport = InViewport;
    StateCache.ScissorRect = InRect;
}

void D3D12PipelineStateCache::SetRenderTargets(D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, D3D12DepthStencilView* InDepthStencil)
{
    // Render targets
    {
        ZeroMemory(StateCache.RenderTargets, sizeof(StateCache.RenderTargets));
        memcpy_s(StateCache.RenderTargets, sizeof(D3D12RenderTargetView*) * InNumRenderTargets, InRenderTargets, sizeof(D3D12RenderTargetView*) * InNumRenderTargets);
        StateCache.NumActivatedRenderTargets = InNumRenderTargets;
    }
    // Depth-stencil
    {
		StateCache.DepthStencil = InDepthStencil;
    }
}

void D3D12PipelineStateCache::SetStreamResource(D3D12Buffer* InVertexBuffer, uint32_t StreamIndex, const UINT InIndicesSize)
{
	// Vertex
	D3D12_VERTEX_BUFFER_VIEW& CurrentVertexCache = StateCache.VertexBufferCache.CurrentVertexBufferView[StreamIndex];
	{
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = InVertexBuffer ? InVertexBuffer->Get()->GetGPUVirtualAddress() : 0;
		view.StrideInBytes = InVertexBuffer->GetStride();
		view.SizeInBytes = InVertexBuffer ? InVertexBuffer->GetSize() : 0;

        if (view.BufferLocation != CurrentVertexCache.BufferLocation ||
			view.StrideInBytes != CurrentVertexCache.StrideInBytes ||
			view.SizeInBytes != CurrentVertexCache.SizeInBytes)
		{
			memcpy_s(&CurrentVertexCache, sizeof(CurrentVertexCache), &view, sizeof(view));
		}
		//else
		//{
  //          CurrentVertexCache.BufferLocation = CurrentVertexCache.BufferLocation + view.BufferLocation;
  //          CurrentVertexCache.StrideInBytes = view.StrideInBytes;
  //          CurrentVertexCache.SizeInBytes = view.SizeInBytes;
		//}
	}

	// Index
	if (InIndicesSize != 0)
	{
		D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = CurrentVertexCache.BufferLocation + CurrentVertexCache.SizeInBytes;
        view.Format = DXGI_FORMAT_R32_UINT;
        view.SizeInBytes = InIndicesSize;

		D3D12_INDEX_BUFFER_VIEW& CurrentIndexCache = StateCache.IndexBufferCache.CurrentIndexBufferView[StreamIndex];

		if (view.BufferLocation != CurrentIndexCache.BufferLocation ||
			view.Format != CurrentIndexCache.Format ||
			view.SizeInBytes != CurrentIndexCache.SizeInBytes)
		{
            //CurrentIndexCache.BufferLocation = CurrentIndexCache.BufferLocation + CurrentVertexCache.BufferLocation + view.BufferLocation;
            //CurrentIndexCache.Format = view.Format;
            //CurrentIndexCache.SizeInBytes = view.SizeInBytes;
			memcpy_s(&CurrentIndexCache, sizeof(CurrentIndexCache), &view, sizeof(view));
		}
		//else
		//{
		//	CurrentIndexCache.BufferLocation = CurrentIndexCache.BufferLocation + virtualAddress + view.BufferLocation;
		//	CurrentIndexCache.Format = view.Format;
		//	CurrentIndexCache.SizeInBytes = view.SizeInBytes;
		//}
	}
}

void D3D12PipelineStateCache::CreateAndAddCache(const D3D12GraphicsPipelineState::Desc& InDesc)
{
	std::shared_ptr<D3D12PipelineState> pipelineState = std::make_shared<D3D12PipelineState>(GetParent(), InDesc);
	if (pipelineState)
	{
		PipelineStateCaches.emplace(InDesc, pipelineState);
	}

	// test
	StateCache.CurrentPipelineState = pipelineState;
}

std::weak_ptr<D3D12PipelineState> D3D12PipelineStateCache::FindCache(const D3D12GraphicsPipelineState::Desc& InDesc)
{
	auto it = PipelineStateCaches.find(InDesc);
	if (it != PipelineStateCaches.cend())
	{
		return it->second;
	}
	return {};
}

std::weak_ptr<D3D12PipelineState> D3D12PipelineStateCache::GetCurrentStateCache() const
{
	return StateCache.CurrentPipelineState;
}
