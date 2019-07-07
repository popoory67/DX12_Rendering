#include "stdafx.h"
#include "FrameResource.h"
#include "D3DApp.h"
#include "UploadBuffer.h"

FrameResource::FrameResource(ID3D12Device* pDevice, UINT passCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CommandListAllocator.GetAddressOf())));

	PassConstBuffer = std::make_unique<UploadBuffer<CommandListResource::PassConstants>>(pDevice, passCount, true);
	ObjectConstBuffer = std::make_unique<UploadBuffer<CommandListResource::ObjectConstants>>(pDevice, objectCount, true);
	MaterialConstBuffer = std::make_unique<UploadBuffer<MaterialConstants>>(pDevice, materialCount, true);
}

FrameResource::~FrameResource()
{

}


