#pragma once
#include "CommnadList.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <stdint.h>
#include <comdef.h>
#include "d3dx12.h"
#include "D3DUtil.h"
#include "GameTimer.h"
#include "FrameResource.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

//struct ID3D12GraphicsCommandList;

class D3D12CommandList : public CommandListBase
{
public:
	D3D12CommandList() = delete;
	D3D12CommandList(ComPtr<ID3D12GraphicsCommandList>& InCommandList, ComPtr<ID3D12Device>& InD3D12Device);

	ComPtr<ID3D12GraphicsCommandList>& Get() { return CommandList; }

	// 원래는 base에 있어야하는데 D3D12용 인터페이스라서 일단 여기에 둠
protected:
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap = nullptr;
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)
	//std::vector<RenderItem*> RenderItems;

private:
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandListAllocator;
};

class D3D12CommandListExecutor : public CommandListExecutor
{
public:
	D3D12CommandListExecutor() = delete;
	D3D12CommandListExecutor(ComPtr<ID3D12Device>& InD3D12Device);

	void Execute(CommandListBase& InCommandList) override;
	void FlushCommands() override;

	ComPtr<ID3D12CommandQueue>& GetExecutor() { return CommandQueue; }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12Fence> Fence;

	UINT64 CurrentFenceCount = 0;
};