#pragma once

#include "D3D12Device.h"
#include "D3DUtil.h"
#include "UploadBuffer.h"
#include "../Util/MathHelper.h"
#include "Light.h"

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};

template<class T>
class ConstantResource
{
public:
	ConstantResource() = delete;
	ConstantResource(const ConstantResource& rhs) = delete;
	ConstantResource& operator=(const ConstantResource& rhs) = delete;

	template<class T>
	ConstantResource(class D3D12Device* InDevice, UINT elementCount)
	{
		assert(InDevice);

		// 수정 필요
		//InDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, CommandListAllocator);

		ConstBuffer = std::make_unique<UploadBuffer<T>>(InDevice, elementCount, true);
	}

	~ConstantResource() {}

	UploadBuffer<T>* Get() { return ConstBuffer.get(); }

	// We cannot reset the allocator until the GPU is done processing the commands.
	// So each frame needs their own allocator.
	ComPtr<ID3D12CommandAllocator> CommandListAllocator;

	// We cannot update a cbuffer until the GPU is done processing the commands that reference it.
	// So each frame needs their own cbuffers.
	std::unique_ptr<UploadBuffer<T>> ConstBuffer = nullptr;

	// This lets us check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;
};