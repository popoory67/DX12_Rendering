#include "stdafx.h"
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include <d3d12.h>

D3D12RenderInterface::D3D12RenderInterface()
{
	Device = new D3D12Device();
	if (Device)
	{
		CmdListExecutor = new D3D12CommandListExecutor(Device);
	}
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

void D3D12RenderInterface::CreateRenderTarget()
{
	D3D12Descriptor* pRenderTargetDesc = new D3D12Descriptor(Device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (pRenderTargetDesc && SwapChain)
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChain->GetSwapChainBufferCount();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pRenderTargetDesc->GetDescriptor().GetAddressOf())));
	}
}

void D3D12RenderInterface::CreateDepthStencil()
{
	D3D12Descriptor* pDepthStencilDesc = new D3D12Descriptor(Device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (pDepthStencilDesc)
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;

		ThrowIfFailed(Device->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(pDepthStencilDesc->GetDescriptor().GetAddressOf())));
	}
}

void D3D12RenderInterface::CreateShaderBuffer()
{
	D3D12Descriptor* pShaderResourceDesc = new D3D12Descriptor(Device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (pShaderResourceDesc)
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(Device->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pShaderResourceDesc->GetDescriptor())));
	}
}

void D3D12RenderInterface::CreateFrameResources()
{
	assert(Device->GetDevice());

	// 개수가 많으면 자료구조로 관리해야함
	// 멀티쓰레드 개념으로 사용하는것
	CurFrameResource = std::make_unique<FrameResource>(Device->GetDevice().Get(), 1, 1, 1);
}

void D3D12RenderInterface::DrawRenderItems(D3D12CommandList* cmdList)
{
	ComPtr<ID3D12GraphicsCommandList> d3d12CommantList = cmdList->Get();

	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(CommandListResource::ObjectConstants));
	UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = CurFrameResource->ObjectConstBuffer->Resource();
	auto matCB = CurFrameResource->MaterialConstBuffer->Resource();

	// For each render item...
	for (size_t i = 0; i < OpaqueRitems.size(); ++i)
	{
		auto ri = OpaqueRitems[i];

		d3d12CommantList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		d3d12CommantList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		d3d12CommantList->IASetPrimitiveTopology(ri->PrimitiveType);

// 		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
// 		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
// 
// 		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
// 		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;
// 
// 		d3d12CommantList->SetGraphicsRootDescriptorTable(0, tex);
// 		d3d12CommantList->SetGraphicsRootConstantBufferView(1, objCBAddress);
// 		d3d12CommantList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		d3d12CommantList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void D3D12RenderInterface::Update()
{

}