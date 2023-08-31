#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"
#include "D3D12Viewport.h"
#include "D3D12Resource.h"
#include "D3D12Fence.h"
#include "D3D12PipelineState.h"
#include "Commands.h"
#include "DirectXColors.h"

void D3D12CommandList::BeginDrawWindow(RHIViewport* InViewport)
{
	D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
	assert(viewport);

	// Set render target that we will use as a back buffer.
	D3D12RenderTargetView* renderTarget = nullptr;
	viewport->GetRenderTargetView(renderTarget);

	if (renderTarget)
	{
		Reset();

		// Indicate a state transition on the resource usage.
		D3D12Resource* resource = D3D12RHI::Cast(renderTarget->GetTexture());
		assert(resource);

		AddTransition(resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
        FlushTransitions();

		// Test
		////ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

		// Clear the back buffer and depth buffer.
		CommandList->ClearRenderTargetView(renderTarget->GetHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
        SetRenderTargets(renderTarget, 1, nullptr);
	}
}

void D3D12CommandList::EndDrawWindow(RHIViewport* InViewport)
{
	D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
	assert(viewport);

	viewport->Present();
}

void D3D12CommandList::BeginRender()
{

}

void D3D12CommandList::EndRender()
{

}

void D3D12CommandList::ResizeViewport(RHIViewport* InViewport)
{
    D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
    assert(viewport);

	GetStateCache().SetViewport(viewport->GetViewport(), viewport->GetRect());
}
