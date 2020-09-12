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

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

// TODO : 이게 Interface가 되어야함
class D3D12Renderer
{
public:
	D3D12Renderer(const D3D12Renderer& rhs) = delete;
	D3D12Renderer& operator=(const D3D12Renderer& rhs) = delete;
	virtual ~D3D12Renderer() = default;

	static D3D12Renderer& Get()
	{
		if (!Interface)
		{
			Interface = new D3D12Renderer();
		}
		return *Interface;
	}
	
	//class D3D12Device* GetDevice()
	//{
	//	return DeviceChild->GetParent();
	//}

	bool Initialize();
	void Update(GameTimer& InTimer)/* = 0*/; // update datas

	void PreRender();
	void Render(GameTimer& InTimer)/* = 0*/; // update render commands
	void PostRender();
	
	//void AddScene(class Scene* InScene);
	//void SetCurrentScene(int InIndex);

private:
	D3D12Renderer() = default;
	
	//void SwapBackBufferToFrontBuffer();
	//class D3D12Resource* GetCurrentBackBuffer() const;
	//D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	//D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;
	//DXGI_FORMAT GetBackBufferFormat() const;
	//DXGI_FORMAT GetDepthStencilFormat() const;

	void OnResize();

private:
	std::unique_ptr<class D3D12DeviceChild> DeviceChild; // multi gpu 지원 안함
	std::unique_ptr<class D3D12Viewport> Viewport; // TODO : change unique param to array

	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFenceCount = 0;

	static D3D12Renderer* Interface;
	
// 	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
// 	XMFLOAT4X4 mView = MathHelper::Identity4x4();
// 	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
// 
// 	float mTheta = 1.3f*XM_PI;
// 	float mPhi = 0.4f*XM_PI;
// 	float mRadius = 2.5f;
// 
// 	POINT mLastMousePos;

// 	// Render items divided by PSO.
// 	std::vector<RenderItem*> OpaqueRitems;
// 
// 	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
};