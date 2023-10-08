#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3DUtil.h"
#include "Util.h"

D3D12Device::D3D12Device()
{
}

D3D12Device::~D3D12Device()
{
	SafeDelete(CommandListExecutor);
    SafeDelete(ResourceManager);
}

Microsoft::WRL::ComPtr<ID3D12Device> D3D12Device::GetDevice() const
{
    return Device;
}

Microsoft::WRL::ComPtr<IDXGIFactory4> D3D12Device::GetDxgi() const
{
	return DxgiFactory;
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
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(&Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_12_1,
			IID_PPV_ARGS(&Device)));
	}
}

ID3D12CommandQueue* D3D12Device::GetCommandQueue() const
{
	return CommandListExecutor->GetCommandQueue();
}

D3D12CommandListExecutor& D3D12Device::GetCommandListExecutor() const
{
	return *CommandListExecutor;
}

D3D12ResourceManager& D3D12Device::GetResourceManager() const
{
	return *ResourceManager;
}

D3D_ROOT_SIGNATURE_VERSION D3D12Device::GetRootSignatureVersion() const
{
	return RootSignatureVersion;
}

void D3D12Device::Initialize()
{
	CreateDevice();

    CommandListExecutor = new D3D12CommandListExecutor(this);
	{
        CommandListExecutor->Initialize();
	}
    ResourceManager = new D3D12ResourceManager();
	{
        D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureCaps = {};
        rootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureCaps, sizeof(rootSignatureCaps)))
        {
            rootSignatureCaps.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        RootSignatureVersion = rootSignatureCaps.HighestVersion;
	}

    // test
    for (int i = 0; i < 2; ++i)
    {
		std::unique_ptr<D3D12CommandList> commandList = std::make_unique<D3D12CommandList>(this);
        commandList->Initialize();

        GCommandContext.AddCommandList(std::move(commandList));
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
	return Parent->GetDevice().Get();
}

void D3D12ResourceManager::AddRootSignature(int InKey, const std::shared_ptr<D3D12RootSignature>& InSignature)
{
	RootSignatures[InKey] = InSignature;
}
