#include "stdafx.h"
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"


D3D12RenderInterface::D3D12RenderInterface(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	assert(InDevice);
	assert(InCommandList);

	Device = InDevice;

	CmdListExecutor = new D3D12CommandListExecutor(Device);

	SwapChain = new D3D12SwapChain(Device);
	if (SwapChain)
	{
		SwapChain->Create(Device, CmdListExecutor);
	}

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	// test
	D3D12PipelineState* pPipelineState = new D3D12PipelineState(InDevice, GetBackBufferFormat(), GetDepthStencilFormat());
	if (pPipelineState)
	{
		PipelineStates.emplace_back(pPipelineState);
	}
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

DXGI_FORMAT D3D12RenderInterface::GetBackBufferFormat() const
{ 
	assert(SwapChain); 
	return SwapChain->GetBackBufferFormat(); 
}

DXGI_FORMAT D3D12RenderInterface::GetDepthStencilFormat() const
{ 
	assert(DepthStencilBuffer); 
	return DepthStencilBuffer->GetDepthStencilFormat(); 
}

void D3D12RenderInterface::OnResize(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	InCommandList->Reset();

	SwapChain->OnResize(Device);

	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	ExecuteCommandList(InCommandList);
	FlushCommandQueue();
}

void D3D12RenderInterface::ExecuteCommandList(D3D12CommandList* InCommandList)
{
	for (D3D12PipelineState* pPso : PipelineStates)
	{
		InCommandList->SetPipelineState(pPso);
	}

	CmdListExecutor->Execute(InCommandList);
}

void D3D12RenderInterface::FlushCommandQueue() const
{
	CmdListExecutor->FlushCommands();
}

void D3D12RenderInterface::UpdateViewport(class D3D12CommandList* InCommandList)
{
	if (InCommandList && SwapChain)
	{
		InCommandList->Get()->RSSetViewports(1, &SwapChain->GetViewport());
		InCommandList->Get()->RSSetScissorRects(1, &SwapChain->GetRect());
	}
}

void D3D12RenderInterface::SetViewport(class D3DViewportResource& InViewResource)
{
	if (SwapChain)
	{
		D3D12_VIEWPORT& viewport = SwapChain->GetViewport();

		viewport.TopLeftX = InViewResource.TopLeftX;
		viewport.TopLeftY = InViewResource.TopLeftY;
		viewport.Width = InViewResource.Width;
		viewport.Height = InViewResource.Height;
		viewport.MinDepth = InViewResource.MinDepth;
		viewport.MaxDepth = InViewResource.MaxDepth;

		D3D12_RECT& ScissorRect = SwapChain->GetRect();

		ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
	}
}

void D3D12RenderInterface::ReadyToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the back buffer and depth buffer.
	InCommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0);
	InCommandList->ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

	// Specify the buffers we are going to render to.
	InCommandList->SetRenderTargets(1, GetCurrentBackBufferView(), true, GetDepthStencilBufferView());
}

void D3D12RenderInterface::FinishToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(SwapChain->GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

D3D12Resource* D3D12RenderInterface::GetCurrentBackBuffer() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBuffer();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetCurrentBackBufferView() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBufferView();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetDepthStencilBufferView() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilView();
}

void D3D12RenderInterface::SwapBackBufferToFrontBuffer()
{
	assert(SwapChain);
	SwapChain->SwapBackBufferToFrontBuffer();
}