#include "D3D12Commands.h"
#include "D3D12RenderInterface.h"
#include "D3D12Viewport.h"
#include "D3D12Resource.h"
#include "D3D12Fence.h"
#include "DirectXColors.h"

void D3D12CommandList::BeginDrawWindow(RHIViewport* InViewport)
{
	D3D12Viewport* viewport = D3D12RHI::Cast(InViewport);
	assert(viewport);

	// Set render target that we will use as a back buffer.
	const auto& backBuffer = viewport->GetCurrentBackBuffer();
	if (backBuffer)
	{
		// Indicate a state transition on the resource usage.
		AddTransition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		FlushTransitions();

		const auto& backBufferView = viewport->GetCurrentBackBufferView();

		// Clear the back buffer and depth buffer.
		ClearRenderTargetView(backBufferView, DirectX::Colors::LightSteelBlue, 0);
		
		//ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

		// Specify the buffers we are going to render to.
		SetRenderTargets(1, backBufferView, true, /*GetDepthStencilBufferView()*/{});
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