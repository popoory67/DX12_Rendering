#pragma once
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
//#include "MathHelper.h"
#include "D3D12Viewport.h"
#include "RenderInterface.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

class D3D12Device;
class D3D12Resource;
class D3D12DefaultResource;
class D3D12ShaderResource;
class D3D12Descriptor;

class D3D12RHI : public RHI
{
public:
	D3D12RHI();
	virtual ~D3D12RHI();

	constexpr FORCEINLINE D3D12Device* GetCurrentDevice()
	{
		return CurrentDevice;
	}

	template<class RHIResourceType>
	static FORCEINLINE TD3D12Types<RHIResourceType>::ResourceType* Cast(RHIResourceType* InResource)
	{
		return static_cast<TD3D12Types<RHIResourceType>::ResourceType*>(InResource);
	}

	void Initialize() override;
	void Destroy() override;

	RHIViewport* CreateViewport(void* InHWnd, unsigned int InWidth, unsigned int InHeight) override;
	void ResizeViewport(RHIViewport* InViewportRHI) override;

	void FlushCommandQueue() const;
	void ResetCommandList();

	void CreateResource(D3D12Resource* InResource, D3D12_RESOURCE_DESC& InDesc, D3D12_CLEAR_VALUE& InValue);
	void CreateResource(D3D12Resource* InResource, UINT64 InByteSize, D3D12_CLEAR_VALUE& InValue);
	void CreateDefaultBuffer(D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize);
	void CreateRenderTarget(D3D12Resource* InResource, CD3DX12_CPU_DESCRIPTOR_HANDLE& InDescriptorHandle, UINT InDescriptorSize);
	void CreateShaderResource(D3D12ShaderResource* InResource, class D3D12Descriptor* InDescriptor, std::string InName = nullptr, std::wstring InFilePath = nullptr);
	
	void LoadTexture(D3D12ShaderResource* InResource, std::string InName = nullptr, std::wstring InFilePath = nullptr);

	// Resource interfaces
	void CreateDepthStencilView(D3D12Resource* InResource, D3D12Descriptor* InDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& InDepthStencilDesc);

private:

	//std::vector<std::shared_ptr<class D3D12PipelineState>> PipelineStates;

	D3D12Device* CurrentDevice;
};