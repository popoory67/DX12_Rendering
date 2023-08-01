#include "D3D12PipelineState.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12Commands.h"
#include "D3D12BinaryLargeObject.h"
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
}

void D3D12PipelineState::SetVertexShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy_s(&PipelineStateDesc.VS, sizeof(PipelineStateDesc.VS) , &InShaderByte, sizeof(InShaderByte));
}

void D3D12PipelineState::SetPixelShader(D3D12_SHADER_BYTECODE& InShaderByte)
{
	memcpy_s(&PipelineStateDesc.PS, sizeof(PipelineStateDesc.PS), &InShaderByte, sizeof(InShaderByte));
}

D3D12PipelineStateCache::D3D12PipelineStateCache(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{

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

void D3D12PipelineStateCache::CreateAndAddCache(const D3D12GraphicsPipelineState::Desc& InDesc)
{
	std::shared_ptr<D3D12PipelineState> pipelineState = std::make_shared<D3D12PipelineState>(GetParent(), InDesc);
	if (pipelineState)
	{
		PipelineStateCaches.emplace(InDesc, pipelineState);
	}
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
