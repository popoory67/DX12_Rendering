#include "D3D12PipelineState.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12Commands.h"
#include "D3D12RootSignature.h"
#include "D3D12Descriptor.h"
#include "D3DUtil.h"

D3D12PipelineState::D3D12PipelineState(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
}

D3D12PipelineState::~D3D12PipelineState()
{
	PipelineState.Reset();
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC& D3D12PipelineState::GetDesc() const
{
	return PipelineStateDesc;
}

void D3D12PipelineState::BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::Key& InKey, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc)
{
    PipelineStateDesc = InDesc;

    GetDevice()->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));

    ComPtr<ID3DBlob> blob;
    PipelineState->GetCachedBlob(&blob);

    size_t dataSize = blob->GetBufferSize();
    {
        GraphicsPipelineState::PSOStream psoStream(dataSize);
        {
            const char* data = static_cast<const char*>(blob->GetBufferPointer());
            psoStream.assign(data, data + dataSize);

            size_t hashKey = GraphicsPipelineState::GetHash(InKey);
            GraphicsPipelineState::Save(hashKey, psoStream);
        }
    }
}

void D3D12PipelineState::BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::PSOStream& InPSO)
{
}

void D3D12PipelineState::BuildPSO(ComPtr<ID3D12PipelineLibrary1> InPipelineLibrary, const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSO)
{
    // Read in the blob data from disk to avoid compiling it.
    PipelineStateDesc.CachedPSO.pCachedBlob = const_cast<char*>(InPSO.data());
    PipelineStateDesc.CachedPSO.CachedBlobSizeInBytes = InPSO.size();

    // Temporary root signature
    {
        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange = {};
        descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsConstantBufferView(0);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = 9999.0f;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        std::shared_ptr<D3D12RootSignature> rootSignature = std::make_shared<D3D12RootSignature>(GetParent(), rootSignatureDesc);
        GetParent()->GetResourceManager().AddRootSignature(1, rootSignature);

        PipelineStateDesc.pRootSignature = rootSignature->GetInterface();
    }

    // Default input
    std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };

    // Default vertex shader
    std::wstring shaderPath = Util::GetShaderAssetFullPath(L"TestShader_vs.cso");
    std::vector<char> vsShader = Util::GetBinaryData(shaderPath);
    {
        PipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vsShader.data(), vsShader.size());
    }

    // Default fragment shader
    shaderPath = Util::GetShaderAssetFullPath(L"TestShader_fs.cso");
    std::vector<char> fsShader = Util::GetBinaryData(shaderPath);
    {
        PipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(fsShader.data(), fsShader.size());
    }

    PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    PipelineStateDesc.SampleMask = UINT_MAX;
    PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PipelineStateDesc.NumRenderTargets = 1;
    PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // render target is limited to 8.
    PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    PipelineStateDesc.SampleDesc.Count = D3D12Viewport::IsMsaa4xEnabled() ? 4 : 1;
    PipelineStateDesc.SampleDesc.Quality = D3D12Viewport::IsMsaa4xEnabled() ? (D3D12Viewport::GetMsaaQuality() - 1) : 0;

    //std::wstring fileName = GraphicsPipelineState::GetPSOFileName(/*hashKey*/0);
    //InPipelineLibrary->LoadGraphicsPipeline(fileName.c_str(), &PipelineStateDesc, IID_PPV_ARGS(&PipelineState));

    GetDevice()->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));
}

D3D12PipelineStateCache::D3D12PipelineStateCache(D3D12Device* InDevice)
	: D3D12Api(InDevice)
{
	DescriptorCache = std::make_shared<D3D12DescriptorCache>(InDevice);

    GetDevice()->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&PipelineLibrary));
}

D3D12PipelineStateCache::~D3D12PipelineStateCache()
{
    SafeDelete(StateCache.RootSignature);
}

void D3D12PipelineStateCache::IssueCachedResources(D3D12CommandList& InCommandList)
{
	InCommandList->RSSetViewports(1, &StateCache.Viewport);
	InCommandList->RSSetScissorRects(1, &StateCache.ScissorRect);

	DescriptorCache->SetRenderTargets(InCommandList, StateCache.RenderTargets, StateCache.NumActivatedRenderTargets, StateCache.DepthStencil);

    DescriptorCache->SetVertexBuffers(InCommandList, StateCache.VertexBufferCache);
    DescriptorCache->SetIndexBuffers(InCommandList, StateCache.IndexBufferCache);
}

void D3D12PipelineStateCache::SetViewport(const D3D12_VIEWPORT& InViewport, const D3D12_RECT& InRect)
{
	StateCache.Viewport = InViewport;
    StateCache.ScissorRect = InRect;
}

void D3D12PipelineStateCache::SetRenderTargets(D3D12RenderTargetView** InRenderTargets, unsigned int InNumRenderTargets, D3D12DepthStencilView* InDepthStencil)
{
    // Render targets
    {
        ZeroMemory(StateCache.RenderTargets, sizeof(StateCache.RenderTargets));
        memcpy_s(StateCache.RenderTargets, sizeof(D3D12RenderTargetView*) * InNumRenderTargets, InRenderTargets, sizeof(D3D12RenderTargetView*) * InNumRenderTargets);
        StateCache.NumActivatedRenderTargets = InNumRenderTargets;
    }
    // Depth-stencil
    {
		StateCache.DepthStencil = InDepthStencil;
    }
}

void D3D12PipelineStateCache::SetStreamResource(D3D12Buffer* InVertexBuffer, uint32_t StreamIndex, const UINT InIndicesSize)
{
	// Vertex
	D3D12_VERTEX_BUFFER_VIEW& CurrentVertexCache = StateCache.VertexBufferCache.CurrentVertexBufferView[StreamIndex];
	{
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = InVertexBuffer ? InVertexBuffer->Get()->GetGPUVirtualAddress() : 0;
		view.StrideInBytes = InVertexBuffer->GetStride();
		view.SizeInBytes = InVertexBuffer ? InVertexBuffer->GetSize() : 0;

        if (view.BufferLocation != CurrentVertexCache.BufferLocation ||
			view.StrideInBytes != CurrentVertexCache.StrideInBytes ||
			view.SizeInBytes != CurrentVertexCache.SizeInBytes)
		{
			memcpy_s(&CurrentVertexCache, sizeof(CurrentVertexCache), &view, sizeof(view));
		}
	}

	// Index
	if (InIndicesSize != 0)
	{
		D3D12_INDEX_BUFFER_VIEW view;
        view.BufferLocation = CurrentVertexCache.BufferLocation + CurrentVertexCache.SizeInBytes;
        view.Format = DXGI_FORMAT_R32_UINT;
        view.SizeInBytes = InIndicesSize;

		D3D12_INDEX_BUFFER_VIEW& CurrentIndexCache = StateCache.IndexBufferCache.CurrentIndexBufferView[StreamIndex];

		if (view.BufferLocation != CurrentIndexCache.BufferLocation ||
			view.Format != CurrentIndexCache.Format ||
			view.SizeInBytes != CurrentIndexCache.SizeInBytes)
		{
			memcpy_s(&CurrentIndexCache, sizeof(CurrentIndexCache), &view, sizeof(view));
		}
	}
}

void D3D12PipelineStateCache::SetShaderBinding(ShaderBinding& InShaderBinding)
{
	ShaderType type = InShaderBinding.Type;

	switch (type)
	{
	case ShaderType::Vertex:
		//StateCache.CurrentPipelineState->GetDesc().VS = { InShaderBinding.Bytecode };
		StateCache.VS = &InShaderBinding;
		break;
	case ShaderType::Fragment:
		StateCache.FS = &InShaderBinding;
		break;
	}
}

void D3D12PipelineStateCache::CreateAndAddCache(GraphicsPipelineState::Key&& InKey, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc)
{
	std::shared_ptr<D3D12PipelineState> pipelineState = std::make_shared<D3D12PipelineState>(GetParent());
	if (pipelineState)
    {
        pipelineState->BuildPSO(PipelineLibrary, std::move(InKey), InDesc);
        StateCache.CurrentPipelineState = pipelineState;
	}
}

void D3D12PipelineStateCache::SetPipelineCache(const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSOCache)
{
	//std::shared_ptr<D3D12PipelineState> pipelineState = std::make_shared<D3D12PipelineState>(GetParent(), InPSOCache);
    std::shared_ptr<D3D12PipelineState> pipelineState = std::make_shared<D3D12PipelineState>(GetParent());
	if (pipelineState)
    {
        pipelineState->BuildPSO(PipelineLibrary, std::move(InKey), InPSOCache);
		StateCache.CurrentPipelineState = pipelineState;
	}
}

std::weak_ptr<D3D12PipelineState> D3D12PipelineStateCache::GetCurrentStateCache() const
{
	return StateCache.CurrentPipelineState;
}