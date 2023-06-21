#include "stdafx.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"

D3D12Device::D3D12Device()
{
}

D3D12Device::~D3D12Device()
{
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

void D3D12Device::CheckFeatureSupport(D3D12_FEATURE InFeature, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS& InMultisampleQualityLevels)
{
	ThrowIfFailed(Device->CheckFeatureSupport(InFeature, &InMultisampleQualityLevels, sizeof(InMultisampleQualityLevels)));
}

void D3D12Device::Initialize()
{
	CreateDevice();

	CommandList = new D3D12CommandList();
	CommandList->Initialize(this);

	CommandListExecutor = new D3D12CommandListExecutor();
	CommandListExecutor->Initialize(this);

	GCommandContext.SetCommandList(CommandList);
}
