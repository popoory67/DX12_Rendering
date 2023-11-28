#pragma once
#include "D3D12Resource.h"
#include "D3D12View.h"
#include "RenderResource.h"
#include "PipelineState.h"
#include <type_traits>
#include <d3d12.h>
#include <unordered_map>

class D3D12PipelineState : public D3D12Api, public PipelineStateCache
{
public:
	D3D12PipelineState() = delete;
	explicit D3D12PipelineState(class D3D12Device* InDevice);
	virtual ~D3D12PipelineState();

	ComPtr<ID3D12PipelineState> Get() { return PipelineState; }
	ID3D12PipelineState* GetInterface() { return PipelineState.Get(); }

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetDesc() const;

	void BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::Key& InKey, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc);
	void BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::PSOStream& InPSO);
	void BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSO);

private:
	ComPtr<ID3D12PipelineState> PipelineState = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
};

// D3D12_INPUT_ELEMENT_DESC should not exceed the value of MAX_VERTEX_SLOT_COUNT
#define MAX_VERTEX_SLOT_COUNT D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT

struct D3D12VertexBufferCache
{
	D3D12_VERTEX_BUFFER_VIEW CurrentVertexBufferView[MAX_VERTEX_SLOT_COUNT];
};

struct D3D12IndexBufferCache
{
	D3D12_INDEX_BUFFER_VIEW CurrentIndexBufferView[MAX_VERTEX_SLOT_COUNT];
};

class D3D12PipelineStateCache : public D3D12Api
{
public:
	D3D12PipelineStateCache() = delete;
	explicit D3D12PipelineStateCache(class D3D12Device* InDevice);
	virtual ~D3D12PipelineStateCache();

	// Process
	void IssueCachedResources(class D3D12CommandList& InCommandList);

	// Cache resources
	void SetViewport(const D3D12_VIEWPORT& InViewport, const D3D12_RECT& InRect);
	void SetRenderTargets(D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, D3D12DepthStencilView* InDepthStencil);
	void SetStreamResource(D3D12Buffer* InVertexBuffer, uint32_t StreamIndex, const UINT InIndicesSize = 0);
	void SetShaderBinding(ShaderBinding& InShaderBinding);

	void CreateAndAddCache(GraphicsPipelineState::Key&& InKey, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc);
	void SetPipelineCache(const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSOCache);
	std::weak_ptr<D3D12PipelineState> GetCurrentStateCache() const;

private:
	std::shared_ptr<class D3D12DescriptorCache> DescriptorCache;

	struct
	{
		std::shared_ptr<D3D12PipelineState> CurrentPipelineState;

		D3D12_VIEWPORT Viewport;
		D3D12_RECT ScissorRect;

		class D3D12RootSignature* RootSignature;

		D3D12RenderTargetView* RenderTargets[MAX_RENDER_TARGETS];
		unsigned int NumActivatedRenderTargets;

		D3D12DepthStencilView* DepthStencil;

		D3D12VertexBufferCache VertexBufferCache;
		D3D12IndexBufferCache IndexBufferCache;

		ShaderBinding* VS;
		ShaderBinding* FS;

	} StateCache;

	ComPtr<ID3D12PipelineLibrary1> PipelineLibrary = nullptr;
};