#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <unordered_map>
#include "d3dx12.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

// namespace backend {

class D3D12CommandList;
class D3D12CommandListExecutor;
class D3D12PipelineStateCache;
class D3D12ResourceManager;

class D3D12Device
{
public:
	D3D12Device();
	virtual ~D3D12Device();

	ComPtr<ID3D12Device> GetDevice() const;
	ComPtr<IDXGIFactory4> GetDxgi() const;

	ID3D12CommandQueue* GetCommandQueue() const;
	D3D12CommandListExecutor& GetCommandListExecutor() const;
	D3D12ResourceManager& GetResourceManager() const;
	D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion() const;

	void Initialize();

private:
	void CreateDevice();

protected:
	ComPtr<ID3D12Device> Device = nullptr; 
	ComPtr<IDXGIFactory4> DxgiFactory = nullptr;

	D3D12CommandListExecutor* CommandListExecutor = nullptr;

	D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion;
	D3D12ResourceManager* ResourceManager = nullptr;
};

// This class is a bridge to the derived
// and ensure that the derived is able to use D3D12 API.
class D3D12Api
{
public:
	D3D12Api() = delete;
	D3D12Api(const D3D12Api&) = delete;
	D3D12Api& operator=(const D3D12Api&) = delete;

	explicit D3D12Api(D3D12Device* InParent);
	virtual ~D3D12Api();

	ID3D12Device* GetDevice() const;

protected:
	FORCEINLINE D3D12Device* GetParent() const noexcept
	{
		return Parent;
	}

private:
	D3D12Device* Parent = nullptr;
};

class D3D12ResourceManager
{
public:
	D3D12ResourceManager() = default;
	~D3D12ResourceManager() = default;

	void AddRootSignature(int InKey, const std::shared_ptr<class D3D12RootSignature>& InSignature);

	void AddDescriptorHeap(class D3D12Descriptor* InDescriptor);

	void ExecuteHeaps(D3D12CommandList& InCommandList);
	void FlushHeaps();

private:
	std::unordered_map<int, std::shared_ptr<class D3D12RootSignature>> RootSignatures;
	std::vector<ID3D12DescriptorHeap*> Heaps;
};