#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"
#include "D3D12Viewport.h"
#include "D3D12Resource.h"
#include "D3D12Fence.h"
#include "Commands.h"
#include "MathHelper.h"
#include "DirectXColors.h"
#if defined(DEBUG) | defined(_DEBUG)
#include <pix3.h>
#endif

void D3D12CommandList::BeginDrawWindow(RHIViewport* InViewport)
{
	D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
	assert(viewport);

	// Set render target that we will use as a back buffer.
	D3D12RenderTargetView* renderTarget = nullptr;
	viewport->GetRenderTargetView(renderTarget);

	D3D12DepthStencilView* depthStencil = nullptr;
    viewport->GetDepthStencilView(depthStencil);

	if (renderTarget)
	{
        Reset();

        PIXBeginEvent(GetCommandList(), PIX_COLOR(0, 255, 0), "Render");

		// Render target which is used use frame buffer
		{
			D3D12Resource* resource = D3D12RHI::Cast(renderTarget->GetTexture());
			assert(resource);

			AddTransition(resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		// Depth-stencil
		{
            D3D12Resource* resource = D3D12RHI::Cast(depthStencil->GetTexture());
            assert(resource);

            CommandList->ClearDepthStencilView(depthStencil->GetHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
            //AddTransition(resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}

		FlushTransitions();

		// Clear the back buffer and depth buffer.
		CommandList->ClearRenderTargetView(renderTarget->GetHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
        SetRenderTargets(renderTarget, 1, depthStencil);
	}
}

void D3D12CommandList::EndDrawWindow(RHIViewport* InViewport)
{
    PIXEndEvent(GetCommandList());

	D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
	assert(viewport);

	viewport->Present(*this);

    EndFrame();
    WaitForFrameCompletion();
}

void D3D12CommandList::BeginRender()
{
    // test
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc = GetStateCache().GetCurrentStateCache().lock()->GetDesc();
    CommandList->SetGraphicsRootSignature(desc.pRootSignature);
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

