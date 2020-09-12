#pragma once
#include <d3d12.h>
#include "D3D12Resource.h"

using namespace Microsoft::WRL;

// TODO : Graphics/Compute
class D3D12PipelineState
{
public:
	D3D12PipelineState() = delete;
	D3D12PipelineState(class D3D12Device* InDevice, DXGI_FORMAT InBackBufferFormat, DXGI_FORMAT InDepthStencilFormat);
	~D3D12PipelineState();

	ComPtr<ID3D12PipelineState> Get() { return PipelineState; }
	ID3D12PipelineState* GetInterface() { return PipelineState.Get(); }

	void SetVertexShader(D3D12_SHADER_BYTECODE& InShaderByte);
	void SetPixelShader(D3D12_SHADER_BYTECODE& InShaderByte);

private:
	ComPtr<ID3D12PipelineState> PipelineState = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
};

#define MAX_VERTEX_SLOT_COUNT D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT

struct D3D12VertexBufferCache
{
	// Vertex
	D3D12_VERTEX_BUFFER_VIEW CurrentVertexBufferView[MAX_VERTEX_SLOT_COUNT];
	D3D12ResourceLocation* ResourceLocation[MAX_VERTEX_SLOT_COUNT];
	int32_t MaxVertexIndex = -1;
};

struct D3D12IndexBufferCache
{
	
};

class D3D12PipelineStateCache
{
public:
	D3D12PipelineStateCache() = default;
	~D3D12PipelineStateCache() = default;
	
	void SetStreamSource(class D3D12VertexBuffer* InVertexBuffer, uint32_t StreamIndex, uint32_t InStride, uint32_t InOffset = 0);
	void SetIndexBuffer();
	
private:
	
	struct
	{
		struct
		{
			std::unique_ptr<D3D12PipelineState> CurrentPSO;

			D3D12VertexBufferCache VertexBufferCache;
		} Graphics;

		struct
		{
			std::unique_ptr<D3D12PipelineState> CurrentPSO;

		} Common;
		
	} PipelineStateCache;
};