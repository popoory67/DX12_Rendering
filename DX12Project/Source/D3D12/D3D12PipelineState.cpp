#include "stdafx.h"
#include "D3D12PipelineState.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12Commands.h"
#include "D3D12BinaryLargeObject.h"
#include "D3D12RootSignature.h"

D3D12PipelineState::D3D12PipelineState(class D3D12Device* InDevice, DXGI_FORMAT InBackBufferFormat, DXGI_FORMAT InDepthStencilFormat)
{
	assert(InDevice);

	D3D12VertexShaderObject* pDefaultVS = new D3D12VertexShaderObject();
	assert(pDefaultVS);

	D3D12PixelShaderObject* pDefaultPS = new D3D12PixelShaderObject();
	assert(pDefaultPS);

	D3D12RootSignature* pRootSignature = new D3D12RootSignature();
	assert(pRootSignature);

	pRootSignature->SetRootSignature(InDevice); // test

	// PSO for opaque objects.
	// The opaque is default.
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
	PipelineStateDesc.pRootSignature = pRootSignature->GetInterface();
	PipelineStateDesc.VS =
	{
		pDefaultVS->GetBufferPointer(),
		pDefaultVS->GetBufferSize()
	};
	PipelineStateDesc.PS =
	{
		pDefaultPS->GetBufferPointer(),
		pDefaultPS->GetBufferSize()
	};
	PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = InBackBufferFormat;
	PipelineStateDesc.SampleDesc.Count = D3D12Viewport::IsMsaa4xEnabled() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = D3D12Viewport::IsMsaa4xEnabled() ? (D3D12Viewport::GetMsaaQuality() - 1) : 0;
	PipelineStateDesc.DSVFormat = InDepthStencilFormat;

	InDevice->CreateGraphicsPipelineState(PipelineState, PipelineStateDesc);
}

D3D12PipelineState::~D3D12PipelineState()
{

}

void D3D12PipelineState::SetVertexShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy_s(&PipelineStateDesc.VS, sizeof(PipelineStateDesc.VS) , &InShaderByte, sizeof(InShaderByte));
}

void D3D12PipelineState::SetPixelShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy_s(&PipelineStateDesc.PS, sizeof(PipelineStateDesc.PS), &InShaderByte, sizeof(InShaderByte));
}

void D3D12PipelineStateCache::SetStreamSource(D3D12VertexBuffer* InVertexBuffer, uint32_t StreamIndex, uint32_t InStride, uint32_t InOffset)
{
	__declspec(align(16)) D3D12_VERTEX_BUFFER_VIEW view;
	view.BufferLocation = InVertexBuffer ? InVertexBuffer->GetGPUVirtualAddress() + InOffset : 0;
	view.StrideInBytes = InStride;
	view.SizeInBytes = InVertexBuffer ? InVertexBuffer->GetSize() - InOffset : 0;

	auto& CurrentVertexCache = PipelineStateCache.Graphics.VertexBufferCache.CurrentVertexBufferView[StreamIndex];

	if (view.BufferLocation != CurrentVertexCache.BufferLocation)
	{
		if (!InVertexBuffer)
		{
			memcpy_s(&CurrentVertexCache, sizeof(CurrentVertexCache), &view, sizeof(view));
		}
	}
}

void D3D12PipelineStateCache::SetIndexBuffer()
{
}
