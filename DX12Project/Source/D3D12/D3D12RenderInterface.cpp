#include "stdafx.h"
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"


D3D12RenderInterface::D3D12RenderInterface(D3D12Device* InDevice)
{
	CommandList.reset(new D3D12CommandList(InDevice));
	CmdListExecutor.reset(new D3D12CommandListExecutor(InDevice));

	// test
	//D3D12PipelineState* pPipelineState = new D3D12PipelineState(InDevice, GetBackBufferFormat(), GetDepthStencilFormat());
	//if (pPipelineState)
	//{
	//	PipelineStates.emplace_back(pPipelineState);
	//}
}

D3D12RenderInterface::~D3D12RenderInterface()
{

}

//void D3D12RenderInterface::OnResize()
//{
//	assert(CommandList);
//
//	CommandList->Reset();
//
////	SwapChain->OnResize();
//
//	ExecuteCommandList();
//	FlushCommandQueue();
//}

void D3D12RenderInterface::ResetCommandList()
{
	assert(CommandList);
	CommandList->Reset();
}

void D3D12RenderInterface::ExecuteCommandList()
{
	assert(CommandList);

	for (D3D12PipelineState* pPso : PipelineStates)
	{
		CommandList->SetPipelineState(pPso);
	}

	CmdListExecutor->Execute(CommandList.get());
}

void D3D12RenderInterface::FlushCommandQueue() const
{
	assert(CmdListExecutor);
	CmdListExecutor->FlushCommands();
}

void D3D12RenderInterface::UpdateViewport(D3D12SwapChain* InSwapChain)
{
	assert(CommandList);
	assert(InSwapChain);

	CommandList->Get()->RSSetViewports(1, &InSwapChain->GetViewport());
	CommandList->Get()->RSSetScissorRects(1, &InSwapChain->GetRect());
}

void D3D12RenderInterface::SetViewport(D3DViewportResource& InViewResource, D3D12SwapChain* InSwapChain)
{
	assert(InSwapChain);

	D3D12_VIEWPORT& viewport = InSwapChain->GetViewport();

	viewport.TopLeftX = InViewResource.TopLeftX;
	viewport.TopLeftY = InViewResource.TopLeftY;
	viewport.Width = InViewResource.Width;
	viewport.Height = InViewResource.Height;
	viewport.MinDepth = InViewResource.MinDepth;
	viewport.MaxDepth = InViewResource.MaxDepth;

	D3D12_RECT& ScissorRect = InSwapChain->GetRect();

	ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
}

void D3D12RenderInterface::ReadyToRenderTarget(D3D12Resource* InBackBuffer, D3D12_CPU_DESCRIPTOR_HANDLE InBackBufferView, D3D12_CPU_DESCRIPTOR_HANDLE InDepthStencilBuffer)
{
	assert(CommandList);

	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(InBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the back buffer and depth buffer.
	CommandList->ClearRenderTargetView(InBackBufferView, Colors::LightSteelBlue, 0);
	CommandList->ClearDepthStencilView(InDepthStencilBuffer, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

	// Specify the buffers we are going to render to.
	CommandList->SetRenderTargets(1, InBackBufferView, true, InDepthStencilBuffer);
}

void D3D12RenderInterface::FinishToRenderTarget(D3D12Resource* InBackBuffer)
{
	assert(CommandList);
	assert(InBackBuffer);

	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(InBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}