#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12Fence.h"
#include "D3D12Descriptor.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3DUtil.h"
#include <optional>

D3D12CommandList::D3D12CommandList(D3D12Device* InDevice)
	: D3D12Api(InDevice)
	, PipelineStateCache(InDevice)
{
   
}

D3D12CommandList::~D3D12CommandList()
{
    SafeDelete(CommandListAllocator);
    SafeDelete(Fence);
}

void D3D12CommandList::Reset()
{
	CommandListAllocator->Reset();

    std::shared_ptr<D3D12PipelineState> cache = GetStateCache().GetCurrentStateCache().lock();
	ThrowIfFailed(CommandList->Reset(CommandListAllocator->Get(), cache->GetInterface()));

	bClosed = false;
}

void D3D12CommandList::Close()
{
	if (!bClosed)
	{
		FlushTransitions();

		ThrowIfFailed(CommandList->Close());

		bClosed = true;
	}
}

ID3D12CommandList* D3D12CommandList::GetCommandLists()
{
	assert(CommandList || CommandList.Get());
	return CommandList.Get();
}

D3D12PipelineStateCache& D3D12CommandList::GetStateCache()
{
    return PipelineStateCache;
}

void D3D12CommandList::Initialize()
{
	CreateCommandList(GetParent());

    // TODO
   // The test code that I write down below has to be separated other classes and stored as a CSV file.
   // What I need to do is to create a reader/writer for an Excel file.
   // Additionally, pipeline state object will read the file on the step of loading resources.
    {
        // root signature
        CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[3];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);

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
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // we need PSO cache
        ComPtr<ID3DBlob> byteCode = nullptr;
        ComPtr<ID3DBlob> byteCode2 = nullptr;
        D3D12_SHADER_BYTECODE VS;
        D3D12_SHADER_BYTECODE PS;
        {
            UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
            compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

            HRESULT hr = S_OK;
            ComPtr<ID3DBlob> errors;

            hr = D3DCompileFromFile(L"Shaders\\TestShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", compileFlags, 0, &byteCode, &errors);

            VS = CD3DX12_SHADER_BYTECODE(byteCode->GetBufferPointer(), byteCode->GetBufferSize());

            hr = D3DCompileFromFile(L"Shaders\\TestShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", compileFlags, 0, &byteCode2, &errors);

            PS = CD3DX12_SHADER_BYTECODE(byteCode2->GetBufferPointer(), byteCode2->GetBufferSize());
        }

        D3D12GraphicsPipelineState::Desc PipelineStateDesc{};

        PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
        PipelineStateDesc.pRootSignature = rootSignature->GetInterface();
        PipelineStateDesc.VS = VS;//D3D12::CompileShader(L"Shaders\\TestShader.hlsl", nullptr, "VS", "vs_5_1");
        PipelineStateDesc.PS = PS;// D3D12::CompileShader(L"Shaders\\TestShader.hlsl", nullptr, "PS", "ps_5_1");
        PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        PipelineStateDesc.SampleMask = UINT_MAX;
        PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        PipelineStateDesc.NumRenderTargets = 1;
        PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        PipelineStateDesc.SampleDesc.Count = D3D12Viewport::IsMsaa4xEnabled() ? 4 : 1;
        PipelineStateDesc.SampleDesc.Quality = D3D12Viewport::IsMsaa4xEnabled() ? (D3D12Viewport::GetMsaaQuality() - 1) : 0;
        //PipelineStateDesc.DSVFormat = InDepthStencilFormat;

        GetStateCache().CreateAndAddCache(PipelineStateDesc);
    }
}

void D3D12CommandList::SetRenderTargets(RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTarget, RHIResource* InDepthStencil)
{
    D3D12RenderTargetView* renderTargetView = D3D12RHI::Cast(InRenderTargets);
    //D3D12DepthStencilView* depthStencilView = static_cast<D3D12DepthStencilView*>(InDepthStencil);

	GetStateCache().SetRenderTargets(&renderTargetView, InNumRenderTarget, nullptr/*depthStencilView*/);
}

void D3D12CommandList::SetStreamResource(std::shared_ptr<RHIResource> InVertexBuffer)
{
    std::shared_ptr<D3D12Buffer> vertexBuffer = std::static_pointer_cast<D3D12Buffer>(InVertexBuffer);
    GetStateCache().SetStreamResource(vertexBuffer, 0, vertexBuffer->GetStride(), 0);
}

void D3D12CommandList::DrawPrimitive(unsigned int InNumVertices, unsigned int InNumInstances, unsigned int InStartIndex, unsigned int InStartInstance)
{
    // test
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc = GetStateCache().GetCurrentStateCache().lock()->GetDesc();
    CommandList->SetGraphicsRootSignature(desc.pRootSignature);

	GetStateCache().IssueCachedResources();
    CommandList->DrawInstanced(InNumVertices, InNumInstances, InStartIndex, InStartInstance);
}

void D3D12CommandList::DrawIndexedInstanced(std::shared_ptr<class RHIResource> InVertexBuffer, unsigned int InNumIndices, unsigned int InNumInstances, unsigned int InStartIndex, int InStartVertex, unsigned int InStartInstance)
{
	GetStateCache().IssueCachedResources();
    CommandList->DrawIndexedInstanced(InNumIndices, InNumInstances, InStartIndex, InStartVertex, InStartInstance);
}

void D3D12CommandList::AddTransition(D3D12Resource* InResource, const D3D12_RESOURCE_STATES& InAfterState)
{
	assert(InResource);
	
	Barriers.emplace_back(std::move(CD3DX12_RESOURCE_BARRIER::Transition(InResource->GetResource(), InResource->GetResourceState(), InAfterState)));

	InResource->SetResourceState(InAfterState);
}

void D3D12CommandList::FlushTransitions()
{
	CommandList->ResourceBarrier(Barriers.size(), Barriers.data());
	
	Barriers.clear();
}

void D3D12CommandList::ClearDepthStencilView(std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> InDescriptorHandle, D3D12_CLEAR_FLAGS ClearFlags, float InDepthValue, UINT8 InStencil, UINT InNumRects, const D3D12_RECT* InRect/* = nullptr*/)
{
	CommandList->ClearDepthStencilView(InDescriptorHandle.value(), ClearFlags, InDepthValue, InStencil, InNumRects, InRect);
}

void D3D12CommandList::AddDescriptorHeap(D3D12Descriptor* InDescriptor)
{
	assert(InDescriptor);

	Heaps.emplace_back(InDescriptor->Get());
}

void D3D12CommandList::ExecuteHeaps()
{
	if (!Heaps.empty())
		CommandList->SetDescriptorHeaps((UINT)Heaps.size(), &(*Heaps.begin()));
}

void D3D12CommandList::FlushHeaps()
{
	Heaps.clear();
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
	CommandList->Close();
}