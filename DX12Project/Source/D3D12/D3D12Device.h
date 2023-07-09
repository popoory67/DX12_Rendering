#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

// namespace backend {

class D3D12CommandList;
class D3D12CommandListExecutor;
class D3D12PipelineStateCache;

class D3D12Device
{
public:
	D3D12Device();
	virtual ~D3D12Device();

	ComPtr<ID3D12Device> GetDevice() const 
	{ 
		ReturnCheckAssert(Device); 
	}

	ComPtr<IDXGIFactory4> GetDxgi() const
	{
		ReturnCheckAssert(DxgiFactory);
	}

	ID3D12CommandQueue* GetCommandQueue() const;
	D3D12CommandList& GetCommandList() const;
	D3D12CommandListExecutor& GetCommandListExecutor() const;
	D3D12PipelineStateCache& GetPSOCache() const;
	D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion() const;

	void Initialize();

private:
	void CreateDevice();

protected:
	ComPtr<ID3D12Device> Device = nullptr; 
	ComPtr<IDXGIFactory4> DxgiFactory = nullptr;

	D3D12CommandListExecutor* CommandListExecutor = nullptr;
	D3D12CommandList* CommandList = nullptr;

	D3D12PipelineStateCache* PipelineStateCache = nullptr;

	D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion;
};

// this class can just make sure that it's a D3D12 api.
class D3D12Api
{
public:
	D3D12Api() = delete;
	D3D12Api(const D3D12Api&) = delete;
	D3D12Api& operator=(const D3D12Api&) = delete;

	explicit D3D12Api(D3D12Device* InParent) : Parent(InParent) 
	{
		assert(InParent);
	}

	ID3D12Device* GetDevice() const
	{
		assert(Parent->GetDevice());
		ReturnCheckAssert(Parent->GetDevice().Get());
	}

protected:
	inline D3D12Device* GetParent() const noexcept
	{
		return Parent;
	}

private:
	D3D12Device* Parent = nullptr;
};