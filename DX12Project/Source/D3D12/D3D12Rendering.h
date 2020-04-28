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

class D3D12Renderer
{
public:

	D3D12Renderer(const D3D12Renderer& rhs) = delete;
	D3D12Renderer& operator=(const D3D12Renderer& rhs) = delete;
	virtual ~D3D12Renderer();

	static D3D12Renderer& GetInstance();

	virtual bool Initialize();
	virtual void Update(GameTimer& InTimer)/* = 0*/; // update datas
	virtual void Render(GameTimer& InTimer)/* = 0*/; // update render commands

	void AddScene(class Scene* InScene);
	void SetCurrentScene(int InIndex);

private:

	D3D12Renderer();

private:

	static D3D12Renderer* Instance;

	std::unique_ptr<class D3D12RenderInterface> RenderInterface;
	std::unique_ptr<class SceneRenderer> Renderer;

	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFenceCount = 0;

	class D3D12CommandList* CommandList; // юс╫ц

	std::map<unsigned int, std::shared_ptr<class Scene>> SceneList;
	unsigned IndexCount = 0;
	int CurrentSceneIndex = -1;

// 	ComPtr<ID3D12PipelineState> OpaquePipelineStateObject = nullptr;
// 
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