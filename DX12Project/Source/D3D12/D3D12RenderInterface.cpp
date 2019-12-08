#include "stdafx.h"
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"
#include <d3d12.h>

D3D12RenderInterface::D3D12RenderInterface(D3D12Device* pDevice)
{
	Device = pDevice;
	if (Device)
	{
		CmdListExecutor = new D3D12CommandListExecutor(Device);
	}

	SwapChain = new D3D12SwapChain(Device/*RenderTargetDesc*/);
	if (SwapChain)
		SwapChain->Create(Device, CmdListExecutor);

//	RenderTargetDesc = CreateRenderTarget();
	ShaderResourceDesc = CreateShaderBuffer();
//	DepthStencilDesc = CreateDepthStencil();
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

// D3D12Descriptor* D3D12RenderInterface::CreateRenderTarget()
// {
// 	D3D12Descriptor* pRenderTargetDesc = new D3D12Descriptor(Device, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
// 	if (pRenderTargetDesc && SwapChain)
// 	{
// 		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
// 		rtvHeapDesc.NumDescriptors = SwapChain->GetSwapChainBufferCount();
// 		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
// 		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
// 		rtvHeapDesc.NodeMask = 0;
// 
// 		ThrowIfFailed(Device->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pRenderTargetDesc->GetDescriptor().GetAddressOf())));
// 	}
// 	return pRenderTargetDesc;
// }

void D3D12RenderInterface::OnResize(D3D12CommandList* InCommandList)
{
	assert(InCommandList);

	//FlushCommandQueue();

	InCommandList->Reset();

	SwapChain->OnResize(Device);

	if (DepthStencilBuffer && DepthStencilBuffer->Get())
		DepthStencilBuffer->Get().Reset();

	DepthStencilDesc = CreateDepthStencil(InCommandList);

	ExecuteCommandList(InCommandList);
	//FlushCommandQueue();
}

D3D12Descriptor* D3D12RenderInterface::CreateDepthStencil(D3D12CommandList* InCommandList)
{
	assert(Device);
	assert(InCommandList);

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	D3D12Descriptor* pDepthStencilDesc = new D3D12Descriptor(Device, dsvHeapDesc, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (pDepthStencilDesc)
	{
		// 		if (DepthStencilBuffer)
		// 		{
		// 			ThrowIfFailed(Device->GetDevice()->CreateCommittedResource(
		// 				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		// 				D3D12_HEAP_FLAG_NONE,
		// 				&depthStencilDesc,
		// 				D3D12_RESOURCE_STATE_COMMON,
		// 				&optClear,
		// 				IID_PPV_ARGS(DepthStencilBuffer->Get().GetAddressOf())));
		// 		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = SwapChain->GetWidth();
		depthStencilDesc.Height = SwapChain->GetHeight();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = SwapChain->IsMsaa4xEnabled() ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = SwapChain->IsMsaa4xEnabled() ? (SwapChain->GetMsaaQuality() - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		DepthStencilBuffer = new D3D12DepthStencilResource(Device, pDepthStencilDesc, dsvDesc, depthStencilDesc, optClear);
	}

	// Transition the resource from its initial state to be used as a depth buffer.
	if (DepthStencilBuffer)
		InCommandList->ResourceBarrier(DepthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	return pDepthStencilDesc;
}

D3D12Descriptor* D3D12RenderInterface::CreateShaderBuffer()
{
	assert(Device);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	D3D12Descriptor* pShaderResourceDesc = new D3D12Descriptor(Device, srvHeapDesc, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return pShaderResourceDesc;
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

// 이건 app 쪽에다가 옮기기
// void D3D12RenderInterface::Update()
// {
// 
// }

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
// void D3DApp::BuildDescriptorHeaps()
// {
// 	//
// 	// Create the SRV heap.
// 	//
// 	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
// 	srvHeapDesc.NumDescriptors = 1;
// 	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
// 	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
// 	ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&SrvHeap)));
// 
// 	//
// 	// Fill out the heap with actual descriptors.
// 	//
// 	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(SrvHeap->GetCPUDescriptorHandleForHeapStart());
// 
// 	auto woodCrateTex = mTextures["woodCrateTex"]->Resource;
// 
// 	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
// 	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
// 	srvDesc.Format = woodCrateTex->GetDesc().Format;
// 	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
// 	srvDesc.Texture2D.MostDetailedMip = 0;
// 	srvDesc.Texture2D.MipLevels = woodCrateTex->GetDesc().MipLevels;
// 	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
// 
// 	D3D12Device->CreateShaderResourceView(woodCrateTex.Get(), &srvDesc, hDescriptor);
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