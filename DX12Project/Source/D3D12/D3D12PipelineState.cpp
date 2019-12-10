#include "stdafx.h"
#include <d3d12.h>
#include "D3D12PipelineState.h"
#include "D3D12Device.h"
#include "D3D12SwapChain.h"
#include "D3D12Commands.h"

D3D12PipelineState::D3D12PipelineState(class D3D12Device* InDevice, DXGI_FORMAT InBackBufferFormat, DXGI_FORMAT InDepthStencilFormat)
{
	assert(InDevice);

	// PSO for opaque objects.
	// The opaque is default.
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
// 	PipelineStateDesc.pRootSignature = RootSignature.Get(); // ID3D12RootSignature
// 	PipelineStateDesc.VS = // D3D12_SHADER_BYTECODE
// 	{
// 		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), // ID3DBlob
// 		mShaders["standardVS"]->GetBufferSize()
// 	};
// 	PipelineStateDesc.PS =
// 	{
// 		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
// 		mShaders["opaquePS"]->GetBufferSize()
// 	};
	PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = InBackBufferFormat;
	PipelineStateDesc.SampleDesc.Count = D3D12SwapChain::IsMsaa4xEnabled() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = D3D12SwapChain::IsMsaa4xEnabled() ? (D3D12SwapChain::GetMsaaQuality() - 1) : 0;
	PipelineStateDesc.DSVFormat = InDepthStencilFormat;

	InDevice->CreateGraphicsPipelineState(PipelineState, PipelineStateDesc);
}

D3D12PipelineState::~D3D12PipelineState()
{

}

void D3D12PipelineState::SetVertexShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy((void*)&PipelineStateDesc.VS, (void*)&InShaderByte, sizeof(D3D12_SHADER_BYTECODE));
}

void D3D12PipelineState::SetPixelShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy((void*)&PipelineStateDesc.PS, (void*)&InShaderByte, sizeof(D3D12_SHADER_BYTECODE));
}