#pragma once
#include "D3D12Resource.h"
#include "D3D12View.h"
#include "D3D12Descriptor.h"
#include <type_traits>
#include <d3d12.h>
#include <unordered_map>

class D3D12PipelineState : public D3D12Api
{
public:
	D3D12PipelineState() = delete;
	explicit D3D12PipelineState(class D3D12Device* InDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc);
	~D3D12PipelineState();

	ComPtr<ID3D12PipelineState> Get() { return PipelineState; }
	ID3D12PipelineState* GetInterface() { return PipelineState.Get(); }

	void SetVertexShader(const D3D12_SHADER_BYTECODE& InShaderByte);
	void SetPixelShader(const D3D12_SHADER_BYTECODE& InShaderByte);

private:
	ComPtr<ID3D12PipelineState> PipelineState = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
};

#define MAX_VERTEX_SLOT_COUNT D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT

struct D3D12VertexBufferCache
{
	D3D12_VERTEX_BUFFER_VIEW CurrentVertexBufferView[MAX_VERTEX_SLOT_COUNT];
	//D3D12ResourceLocation* ResourceLocation[MAX_VERTEX_SLOT_COUNT];
	int32_t MaxVertexIndex = -1;
};

struct D3D12IndexBufferCache
{
	
};

namespace D3D12GraphicsPipelineState
{
	struct Desc : public D3D12_GRAPHICS_PIPELINE_STATE_DESC
	{
		Desc()
			: D3D12_GRAPHICS_PIPELINE_STATE_DESC()
		{
			// TODO
			// Has it to be a UUID? (IID_PPV_ARGS)
			static int counter = 0;
			UniqueKey = ++counter;
		}

		int UniqueKey;
	};

	struct Hash
	{
		std::size_t operator()(const Desc& InDesc) const
		{
			return std::hash<int>()(InDesc.UniqueKey);
		}
	};

	struct HashCompare
	{
		bool operator()(const Desc& lhs, const Desc& rhs) const
		{
			return lhs.UniqueKey == rhs.UniqueKey;
		}
	};
};

class D3D12PipelineStateCache : public D3D12Api
{
public:
	D3D12PipelineStateCache() = delete;
	explicit D3D12PipelineStateCache(class D3D12Device* InDevice);
	virtual ~D3D12PipelineStateCache() = default;

	// Process
	void IssueCachedResources();

	// Cache resources
	void SetRenderTargets(D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, D3D12DepthStencilView* InDepthStencil);
	void SetStreamResource(std::shared_ptr<class D3D12Buffer>& InVertexBuffer, uint32_t StreamIndex, uint32_t InStride, uint32_t InOffset = 0);

	void CreateAndAddCache(const D3D12GraphicsPipelineState::Desc& InDesc);
	std::weak_ptr<D3D12PipelineState> FindCache(const D3D12GraphicsPipelineState::Desc& InDesc);

private:
	
	std::shared_ptr<D3D12DescriptorCache> DescriptorCache;

	struct
	{
		std::shared_ptr<D3D12PipelineState> CurrentPipelineState;

		D3D12RenderTargetView* RenderTargets[MAX_RENDER_TARGETS];
		unsigned int NumActivatedRenderTargets;

		D3D12DepthStencilView* DepthStencil;

		D3D12VertexBufferCache VertexBufferCache;

	} StateCache;

	std::unordered_map<D3D12GraphicsPipelineState::Desc, std::shared_ptr<D3D12PipelineState>, D3D12GraphicsPipelineState::Hash, D3D12GraphicsPipelineState::HashCompare> PipelineStateCaches;
};