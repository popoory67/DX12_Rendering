#include "stdafx.h"
//#include <d3d12.h>
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"

D3D12RenderInterface::D3D12RenderInterface(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	assert(InDevice);
	assert(InCommandList);

	Device = InDevice;

	CmdListExecutor = new D3D12CommandListExecutor(Device);

	SwapChain = new D3D12SwapChain(Device);
	if (SwapChain)
		SwapChain->Create(Device, CmdListExecutor);

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

void D3D12RenderInterface::OnResize(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	InCommandList->Reset();

	SwapChain->OnResize(Device);

	if (DepthStencilBuffer)
		DepthStencilBuffer->Reset();

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	ExecuteCommandList(InCommandList);
	FlushCommandQueue();
}

void D3D12RenderInterface::ExecuteCommandList(D3D12CommandList* InCommandList) const
{
	CmdListExecutor->Execute(InCommandList);
}

void D3D12RenderInterface::FlushCommandQueue() const
{
	CmdListExecutor->FlushCommands();
}

void D3D12RenderInterface::UpdateViewport(class D3D12CommandList* InCommandList)
{
	if (InCommandList && SwapChain)
	{
		InCommandList->Get()->RSSetViewports(1, &SwapChain->GetViewport());
		InCommandList->Get()->RSSetScissorRects(1, &SwapChain->GetRect());
	}
}
void D3D12RenderInterface::SetViewport(class D3DViewportResource& InViewResource)
{
	if (SwapChain)
	{
		D3D12_VIEWPORT& viewport = SwapChain->GetViewport();

		viewport.TopLeftX = InViewResource.TopLeftX;
		viewport.TopLeftY = InViewResource.TopLeftY;
		viewport.Width = InViewResource.Width;
		viewport.Height = InViewResource.Height;
		viewport.MinDepth = InViewResource.MinDepth;
		viewport.MaxDepth = InViewResource.MaxDepth;

		D3D12_RECT& ScissorRect = SwapChain->GetRect();

		ScissorRect = { 0, 0, (LONG)viewport.Width, (LONG)viewport.Height };
	}
}

void D3D12RenderInterface::ReadyToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the back buffer and depth buffer.
	InCommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0);
	InCommandList->ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0);

	// Specify the buffers we are going to render to.
	InCommandList->SetRenderTargets(1, GetCurrentBackBufferView(), true, GetDepthStencilBufferView());
}

void D3D12RenderInterface::FinishToRenderTarget(class D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	// Indicate a state transition on the resource usage.
	InCommandList->ResourceBarrier(SwapChain->GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

D3D12Resource* D3D12RenderInterface::GetCurrentBackBuffer() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBuffer();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetCurrentBackBufferView() const
{
	assert(SwapChain);
	return SwapChain->GetCurrentBackBufferView();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderInterface::GetDepthStencilBufferView() const
{
	assert(DepthStencilBuffer);
	return DepthStencilBuffer->GetDepthStencilView();
}

void D3D12RenderInterface::SwapBackBufferToFrontBuffer()
{
	assert(SwapChain);
	SwapChain->SwapBackBufferToFrontBuffer();
}

void D3D12RenderInterface::CreateFrameResources()
{
// 	assert(Device->GetDevice());
// 
// 	// 개수가 많으면 자료구조로 관리해야함
// 	// 멀티쓰레드 개념으로 사용하는것
// 	CurFrameResource = std::make_unique<FrameResource>(Device->GetDevice().Get(), 1, 1, 1);
}

void D3D12RenderInterface::DrawRenderItems(D3D12CommandList* InCommandList)
{
// 	ComPtr<ID3D12GraphicsCommandList> d3d12CommantList = cmdList->Get();
// 
// 	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(CommandListResource::ObjectConstants));
// 	UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
// 
// 	auto objectCB = CurFrameResource->ObjectConstBuffer->Resource();
// 	auto matCB = CurFrameResource->MaterialConstBuffer->Resource();
// 
// 	// For each render item...
// 	for (size_t i = 0; i < OpaqueRitems.size(); ++i)
// 	{
// 		auto ri = OpaqueRitems[i];
// 
// 		d3d12CommantList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
// 		d3d12CommantList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
// 		d3d12CommantList->IASetPrimitiveTopology(ri->PrimitiveType);
// 
// // 		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
// // 		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize); // CbvSrvUavDescriptorSize = 32
// // 
// // 		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
// // 		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;
// // 
// // 		d3d12CommantList->SetGraphicsRootDescriptorTable(0, tex);
// // 		d3d12CommantList->SetGraphicsRootConstantBufferView(1, objCBAddress);
// // 		d3d12CommantList->SetGraphicsRootConstantBufferView(3, matCBAddress);
// 
// 		d3d12CommantList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
// 	}
}

// 
//  void D3DApp::BuildRootSignature()
// {
// 	CD3DX12_DESCRIPTOR_RANGE texTable;
// 	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
// 
// 	// Root parameter can be a table, root descriptor or root constants.
// 	CD3DX12_ROOT_PARAMETER slotRootParameter[4];
// 
// 	// Perfomance TIP: Order from most frequent to least frequent.
// 	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
// 	slotRootParameter[1].InitAsConstantBufferView(0);
// 	slotRootParameter[2].InitAsConstantBufferView(1);
// 	slotRootParameter[3].InitAsConstantBufferView(2);
// 
// 	auto staticSamplers = GetStaticSamplers();
// 
// 	// A root signature is an array of root parameters.
// 	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
// 		(UINT)staticSamplers.size(), staticSamplers.data(),
// 		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
// 
// 	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
// 	ComPtr<ID3DBlob> serializedRootSig = nullptr;
// 	ComPtr<ID3DBlob> errorBlob = nullptr;
// 	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
// 		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
// 
// 	if (errorBlob != nullptr)
// 	{
// 		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
// 	}
// 	ThrowIfFailed(hr);
// 
// 	ThrowIfFailed(D3D12Device->CreateRootSignature(
// 		0,
// 		serializedRootSig->GetBufferPointer(),
// 		serializedRootSig->GetBufferSize(),
// 		IID_PPV_ARGS(RootSignature.GetAddressOf())));
// }
// 
// void D3DApp::BuildShadersAndInputLayout()
// {
// 	mShaders["standardVS"] = D3DUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
// 	mShaders["opaquePS"] = D3DUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_0");
// 
// 	mInputLayout =
// 	{
// 		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
// 	};
// }
// 
// void D3DApp::BuildShapeGeometry()
// {
// 	GeometryGenerator geoGen;
// 	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
// 
// 	SubmeshGeometry boxSubmesh;
// 	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
// 	boxSubmesh.StartIndexLocation = 0;
// 	boxSubmesh.BaseVertexLocation = 0;
// 
// 
// 	std::vector<GeometryGenerator::Vertex> vertices(box.Vertices.size());
// 
// 	for (size_t i = 0; i < box.Vertices.size(); ++i)
// 	{
// 		vertices[i].Position = box.Vertices[i].Position;
// 		vertices[i].Normal = box.Vertices[i].Normal;
// 		vertices[i].TexC = box.Vertices[i].TexC;
// 	}
// 
// 	std::vector<std::uint16_t> indices = box.GetIndices16();
// 
// 	const UINT vbByteSize = (UINT)vertices.size() * sizeof(GeometryGenerator::Vertex);
// 	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);
// 
// 	auto geo = std::make_unique<MeshGeometry>();
// 	geo->Name = "boxGeo";
// 
// 	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
// 	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
// 
// 	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
// 	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
// 
// 	geo->VertexBufferGPU = D3DUtil::CreateDefaultBuffer(D3D12Device.Get(),
// 		CommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
// 
// 	geo->IndexBufferGPU = D3DUtil::CreateDefaultBuffer(D3D12Device.Get(),
// 		CommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);
// 
// 	geo->VertexByteStride = sizeof(GeometryGenerator::Vertex);
// 	geo->VertexBufferByteSize = vbByteSize;
// 	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
// 	geo->IndexBufferByteSize = ibByteSize;
// 
// 	geo->DrawArgs["box"] = boxSubmesh;
// 
// 	mGeometries[geo->Name] = std::move(geo);
// }
// 
// void D3DApp::BuildPSOs()
// {
// 	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
// 
// 	//
// 	// PSO for opaque objects.
// 	//
// 	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
// 	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
// 	opaquePsoDesc.pRootSignature = RootSignature.Get();
// 	opaquePsoDesc.VS =
// 	{
// 		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
// 		mShaders["standardVS"]->GetBufferSize()
// 	};
// 	opaquePsoDesc.PS =
// 	{
// 		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
// 		mShaders["opaquePS"]->GetBufferSize()
// 	};
// 	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
// 	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
// 	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
// 	opaquePsoDesc.SampleMask = UINT_MAX;
// 	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
// 	opaquePsoDesc.NumRenderTargets = 1;
// 	opaquePsoDesc.RTVFormats[0] = BackBufferFormat;
// 	opaquePsoDesc.SampleDesc.Count = IsMsaa4xState ? 4 : 1;
// 	opaquePsoDesc.SampleDesc.Quality = IsMsaa4xState ? (Msaa4xQuality - 1) : 0;
// 	opaquePsoDesc.DSVFormat = DepthStencilFormat;
// 	ThrowIfFailed(Device->GetDevice()->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&OpaquePipelineStateObject)));
// }
// 
// void D3DApp::BuildFrameResources()
// {
// 	for (int i = 0; i < gNumFrameResources; ++i)
// 	{
// 		mFrameResources.push_back(std::make_unique<FrameResource>(D3D12Device.Get(),
// 			1, (UINT)AllRitems.size(), (UINT)mMaterials.size()));
// 	}
// }
// 
// // 얘네는 좀 애매함
// void D3DApp::BuildMaterials()
// {
// 	auto woodCrate = std::make_unique<Material>();
// 	woodCrate->Name = "woodCrate";
// 	woodCrate->MatCBIndex = 0;
// 	woodCrate->DiffuseSrvHeapIndex = 0;
// 	woodCrate->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
// 	woodCrate->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
// 	woodCrate->Roughness = 0.2f;
// 
// 	mMaterials["woodCrate"] = std::move(woodCrate);
// }
// 
// void D3DApp::BuildRenderItems()
// {
// 	auto boxRitem = std::make_unique<RenderItem>();
// 	boxRitem->ObjCBIndex = 0;
// 	boxRitem->Mat = mMaterials["woodCrate"].get();
// 	boxRitem->Geo = mGeometries["boxGeo"].get();
// 	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
// 	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
// 	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
// 	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
// 	AllRitems.push_back(std::move(boxRitem));
// 
// 	// All the render items are opaque.
// 	for (auto& e : AllRitems)
// 		OpaqueRitems.push_back(e.get());
// }