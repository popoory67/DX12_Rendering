#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "D3D12Resource.h"
#include "D3D12RootSignature.h"
#include "D3D12PipelineState.h"
#include "D3DUtil.h"
#include <optional>

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
	: D3D12Api(InDevice)
	, PSOCache(InDevice)
{

}

D3D12CommandList::~D3D12CommandList()
{
    Close();

    SafeDelete(Fence);
    SafeDelete(CommandListAllocator);
}

void D3D12CommandList::Reset()
{
    ResourceManager.CleanUp();

	CommandListAllocator->Reset();

    std::shared_ptr<D3D12PipelineState> cache = GetStateCache().GetCurrentStateCache().lock();
	ThrowIfFailed(GetCommandList()->Reset(CommandListAllocator->Get(), cache->GetInterface()));

	bClosed = false;
}

void D3D12CommandList::Close()
{
	if (!bClosed)
	{
		FlushTransitions();

		ThrowIfFailed(GetCommandList()->Close());

		bClosed = true;
	}
}

ID3D12GraphicsCommandList* D3D12CommandList::GetCommandList()
{
	assert(CommandList || CommandList.Get());
	return CommandList.Get();
}

D3D12PipelineStateCache& D3D12CommandList::GetStateCache()
{
    return PSOCache;
}

void D3D12CommandList::Initialize()
{
	CreateCommandList(GetParent());
    
    Fence = new D3D12Fence(GetParent());
    Fence->Signal();

    // Default PSO
    GraphicsPipelineState::Key psoKey{};
    D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc{};
    {
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
        PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        PipelineStateDesc.SampleMask = UINT_MAX;
        PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        PipelineStateDesc.NumRenderTargets = 1;
        PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // render target is limited to 8.
        PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        PipelineStateDesc.SampleDesc.Count = D3D12Viewport::IsMsaa4xEnabled() ? 4 : 1;
        PipelineStateDesc.SampleDesc.Quality = D3D12Viewport::IsMsaa4xEnabled() ? (D3D12Viewport::GetMsaaQuality() - 1) : 0;

        GetStateCache().CreateAndAddCache(std::move(psoKey), PipelineStateDesc);
    }
}

void D3D12CommandList::SetRenderTargets(RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTarget, RHIDepthStencilInfo* InDepthStencil)
{
    D3D12RenderTargetView* renderTargetView = D3D12RHI::Cast(InRenderTargets);
    D3D12DepthStencilView* depthStencilView = D3D12RHI::Cast(InDepthStencil);

	GetStateCache().SetRenderTargets(&renderTargetView, InNumRenderTarget, depthStencilView);
}

void D3D12CommandList::SetStreamResource(RHIResource* InVertexBuffer, const UINT InIndicesSize)
{
    D3D12Buffer* vertexBuffer = static_cast<D3D12Buffer*>(InVertexBuffer);

    GetStateCache().SetStreamResource(vertexBuffer, 0, InIndicesSize);
}

void D3D12CommandList::SetShaderBinding(ShaderBinding& InBinding)
{
    GetStateCache().SetShaderBinding(InBinding);
}

void D3D12CommandList::AddShaderReference(int InIndex, RHIResource* InBuffer)
{
    D3D12Resource* buffer = D3D12RHI::Cast(InBuffer);

    // test
    GetCommandList()->SetGraphicsRootConstantBufferView(0, buffer->GetAddress());
}

void D3D12CommandList::DrawPrimitive(unsigned int InNumVertices, unsigned int InNumInstances, unsigned int InStartIndex, unsigned int InStartInstance)
{
	GetStateCache().IssueCachedResources(*this);
    GetCommandList()->DrawInstanced(InNumVertices, InNumInstances, InStartIndex, InStartInstance);
}

void D3D12CommandList::DrawIndexedInstanced(unsigned int InNumIndices, unsigned int InNumInstances, unsigned int InStartIndex, int InStartVertex, unsigned int InStartInstance)
{
	GetStateCache().IssueCachedResources(*this);
    GetCommandList()->DrawIndexedInstanced(InNumIndices, InNumInstances, InStartIndex, InStartVertex, InStartInstance);
}

uint32_t CalculateRowPitch(uint32_t InWidth, uint32_t InBytesPerPixel) 
{
    uint32_t rowPitch = (InWidth * InBytesPerPixel + (D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);
    return rowPitch;
}

void D3D12CommandList::CopyResourceRegion(RHIResource* InDestResource, RHIResource* InSourceResource)
{
    D3D12Resource* destResource = D3D12RHI::Cast(InDestResource);
    D3D12Resource* sourceResource = D3D12RHI::Cast(InSourceResource);

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
    {
        UINT64 totalSize = 0;
        GetDevice()->GetCopyableFootprints(&sourceResource->GetDesc(), 0, 1, 0, &placedFootprint, nullptr, nullptr, &totalSize);

        assert(placedFootprint.Offset % D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT == 0);

        placedFootprint.Footprint.Format = destResource->GetDesc().Format;
        placedFootprint.Footprint.Width = destResource->GetDesc().Width;
        placedFootprint.Footprint.Height = destResource->GetDesc().Height;
        placedFootprint.Footprint.Depth = destResource->GetDesc().DepthOrArraySize;
        placedFootprint.Footprint.RowPitch = CalculateRowPitch(placedFootprint.Footprint.Width, 4); // 4 bytes
        placedFootprint.Offset = 0;
    }

    CD3DX12_TEXTURE_COPY_LOCATION dest(destResource->GetResource(), 0);
    {
        destResource->SetResourceState(D3D12_RESOURCE_STATE_COPY_DEST);
    }

    CD3DX12_TEXTURE_COPY_LOCATION source(sourceResource->GetResource(), placedFootprint);

    GetCommandList()->CopyTextureRegion(&dest, 0, 0, 0, &source, nullptr);

    AddTransition(destResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void D3D12CommandList::SetPipelineState(const GraphicsPipelineState::Key& InKey, const GraphicsPipelineState::PSOStream& InPSOCache)
{
    GetStateCache().SetPipelineCache(InKey, InPSOCache);
}

void D3D12CommandList::CreateAndAddPipelineState(std::vector<ShaderBinding*>&& InShaders)
{
    GraphicsPipelineState::Key psoKey{};
    D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc{};

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

    // resource
    std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    for (const auto& shader : InShaders)
    {
        ShaderType type = shader->Type;

        switch (type)
        {
        case ShaderType::Vertex:
            psoKey.VertexShader = shader->Hash;
            PipelineStateDesc.VS = { shader->Bytecode, shader->BytecodeLength };
            break;
        case ShaderType::Fragment:
            psoKey.FragmentShader = shader->Hash;
            PipelineStateDesc.PS = { shader->Bytecode, shader->BytecodeLength };
            break;
        }
    }

    PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
    PipelineStateDesc.pRootSignature = rootSignature->GetInterface();
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

    GetStateCache().CreateAndAddCache(std::move(psoKey), PipelineStateDesc);
}

void D3D12CommandList::AddTransition(D3D12Resource* InResource, const D3D12_RESOURCE_STATES& InAfterState)
{
	assert(InResource);
	
	Barriers.emplace_back(std::move(CD3DX12_RESOURCE_BARRIER::Transition(InResource->GetResource(), InResource->GetResourceState(), InAfterState)));

	InResource->SetResourceState(InAfterState);
}

void D3D12CommandList::FlushTransitions()
{
    GetCommandList()->ResourceBarrier(Barriers.size(), Barriers.data());
	
	Barriers.clear();
}

void D3D12CommandList::AddResource(RHIResource* InResource)
{
    ResourceManager.AddResource(InResource);
}

void D3D12CommandList::CreateCommandList(D3D12Device* InDevice)
{
	CommandListAllocator = new D3D12CommandAllocator(InDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

	ThrowIfFailed(InDevice->GetDevice()->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandListAllocator->Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(&CommandList)));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
    GetCommandList()->Close();
}

void D3D12CommandList::WaitForFrameCompletion()
{
    Fence->CpuWait(CurrentFence);
}

void D3D12CommandList::EndFrame()
{
    CurrentFence = Fence->Signal();
}