#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12PipelineState.h"
#include "D3DUtil.h"
#include "Util.h"

// test
#include "D3D12BinaryLargeObject.h"
#include "D3D12RootSignature.h"
#include "D3D12Viewport.h"

D3D12Device::D3D12Device()
{
}

D3D12Device::~D3D12Device()
{
	SafeDelete(CommandListExecutor);
    SafeDelete(CommandList);
    SafeDelete(PipelineStateCache);
}

Microsoft::WRL::ComPtr<ID3D12Device> D3D12Device::GetDevice() const
{
    ReturnCheckAssert(Device);
}

Microsoft::WRL::ComPtr<IDXGIFactory4> D3D12Device::GetDxgi() const
{
    ReturnCheckAssert(DxgiFactory);
}

void D3D12Device::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&Device)));
	}
}

ID3D12CommandQueue* D3D12Device::GetCommandQueue() const
{
	return CommandListExecutor->GetCommandQueue();
}

D3D12CommandList& D3D12Device::GetCommandList() const
{
	return *CommandList;
}

D3D12CommandListExecutor& D3D12Device::GetCommandListExecutor() const
{
	return *CommandListExecutor;
}

D3D12PipelineStateCache& D3D12Device::GetPSOCache() const
{
	return *PipelineStateCache;
}

D3D_ROOT_SIGNATURE_VERSION D3D12Device::GetRootSignatureVersion() const
{
	return RootSignatureVersion;
}

void D3D12Device::Initialize()
{
	CreateDevice();

    CommandList = new D3D12CommandList(this);
    CommandList->Initialize();

    CommandListExecutor = new D3D12CommandListExecutor(this);
    CommandListExecutor->Initialize();

    GCommandContext.SetCommandList(CommandList);

	// TODO
	// The test code that I write down below has to be separated other classes and stored as a CSV file.
	// What I need to do is to create a reader/writer for an Excel file.
	// Additionally, pipeline state object will read the file on the step of loading resources.
	PipelineStateCache = new D3D12PipelineStateCache(this);
	{
		D3D12GraphicsPipelineState::Desc PipelineStateDesc{};

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

        D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureCaps = {};
        rootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureCaps, sizeof(rootSignatureCaps)))
        {
            rootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        RootSignatureVersion = rootSignatureCaps.HighestVersion;

		D3D12RootSignature* pRootSignature = new D3D12RootSignature(this, rootSignatureDesc);
		assert(pRootSignature);

		// resource
		std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout =
		{
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

        D3D12VertexShaderObject* pDefaultVS = new D3D12VertexShaderObject();
        assert(pDefaultVS);

        D3D12PixelShaderObject* pDefaultPS = new D3D12PixelShaderObject();
        assert(pDefaultPS);

        // triangle
        //VertexStream triangleVertices =
        //{
        //    { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        //    { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        //    { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        //};

		//const UINT size = triangleVertices.size();

		PipelineStateDesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
		PipelineStateDesc.pRootSignature = pRootSignature->GetInterface();
		PipelineStateDesc.VS = { pDefaultVS->GetShaderBytecode() };
		PipelineStateDesc.PS = { pDefaultPS->GetShaderBytecode() };
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

		PipelineStateCache->CreateAndAddCache(PipelineStateDesc);
	}
}

D3D12Api::D3D12Api(D3D12Device* InParent)
    : Parent(InParent)
{
    assert(InParent);
}

D3D12Api::~D3D12Api()
{
	Parent = nullptr;
}

ID3D12Device* D3D12Api::GetDevice() const
{
    assert(Parent->GetDevice());
    ReturnCheckAssert(Parent->GetDevice().Get());
}
