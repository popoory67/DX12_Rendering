#include "stdafx.h"
#include "D3DApp.h"
#include <combaseapi.h>
#include "GeometryGenerator.h"
#include "DDSTextureLoader.h"
#include "D3D12Device.h"
#include "D3D12RenderInterface.h"
#include "D3D12SwapChain.h"
#include "D3D12Commands.h"

D3DApp* D3DApp::Instance = nullptr;

D3DApp::D3DApp()
{
	// Only one D3DApp can be constructed.
	assert(Instance == nullptr);
	Instance = this;
}

D3DApp::~D3DApp()
{
}

D3DApp& D3DApp::GetInstance()
{
	if (!Instance)
		Instance = new D3DApp();

	return *Instance;
}

bool D3DApp::Initialize(HWND InWindowHandle)
{
	D3D12Device* pDevice = new D3D12Device(InWindowHandle);
	assert(pDevice);

	CommandList = new D3D12CommandList(pDevice);
	assert(CommandList);

	RenderInterface = new D3D12RenderInterface(pDevice, CommandList);
	assert(RenderInterface);

 	RenderInterface->OnResize(CommandList);

	// Update the viewport transform to cover the client area.
	D3DViewportResource ScreenViewport;
	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = 800.0f;
	ScreenViewport.Height = 600.0f;
	ScreenViewport.MinDepth = 0.0f;
	ScreenViewport.MaxDepth = 1.0f;

	RenderInterface->SetViewport(ScreenViewport);

	// 리소스가 있을때 사용되는거라 일단 주석처리
	// mat, tex에 이용됨
	//CbvSrvUavDescriptorSize = D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	RenderInterface->GetDescriptorHandleIncrementSize();

	//	RenderInterface->CreateFrameResources();

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

// 	LoadTextures();
// 	BuildRootSignature();
// 	BuildDescriptorHeaps();
// 	BuildShadersAndInputLayout();
// 	BuildShapeGeometry();
// 	BuildMaterials();
// 	BuildRenderItems();
// 	BuildFrameResources();
// 	BuildPSOs();

	return true;
}

void D3DApp::Update(const GameTimer& gt)
{
// 	OnKeyboardInput(gt);
// 	UpdateCamera(gt);
// 
// 	// Cycle through the circular frame resource array.
// 	CurFrameResourceIndex = (CurFrameResourceIndex + 1) % gNumFrameResources;
// 	CurFrameResource = mFrameResources[CurFrameResourceIndex].get();
// 
// 	// Has the GPU finished processing the commands of the current frame resource?
// 	// If not, wait until the GPU has completed commands up to this fence point.
// 	if (CurFrameResource->Fence != 0 && Fence->GetCompletedValue() < CurFrameResource->Fence)
// 	{
// 		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
// 		ThrowIfFailed(Fence->SetEventOnCompletion(CurFrameResource->Fence, eventHandle));
// 		WaitForSingleObject(eventHandle, INFINITE);
// 		CloseHandle(eventHandle);
// 	}
// 
// 	AnimateMaterials(gt);
// 	UpdateObjectCBs(gt);
// 	UpdateMaterialCBs(gt);
// 	UpdateMainPassCB(gt);
}

// 1 드로우 = 1 프레임
void D3DApp::Draw(const GameTimer& gt)
{
	// Ready to draw
	CommandList->Reset();

	RenderInterface->UpdateViewport(CommandList);
	RenderInterface->ReadyToRenderTarget(CommandList);

	// --test
//  	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvHeap.Get() };
//  	CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//  
//  	CommandList->SetGraphicsRootSignature(RootSignature.Get());
//  
//  	ID3D12Resource* PassConstBuffer = CurFrameResource->PassConstBuffer->Resource();
//  	CommandList->SetGraphicsRootConstantBufferView(2, PassConstBuffer->GetGPUVirtualAddress()); // 임시 주석처리
// 
// 	DrawRenderItems(CommandList.Get(), OpaqueRitems);

	// End draw
	RenderInterface->FinishToRenderTarget(CommandList);

	// Finish to recode resource
	RenderInterface->ExecuteCommandList(CommandList);
	RenderInterface->SwapBackBufferToFrontBuffer();
	RenderInterface->FlushCommandQueue();
}

// void D3DApp::UpdateCamera(const GameTimer& gt)
// {
// 	// Convert Spherical to Cartesian coordinates.
// 	mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta);
// 	mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta);
// 	mEyePos.y = mRadius * cosf(mPhi);
// 
// 	// Build the view matrix.
// 	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
// 	XMVECTOR target = XMVectorZero();
// 	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
// 
// 	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
// 	XMStoreFloat4x4(&mView, view);
// }
// 
// void D3DApp::AnimateMaterials(const GameTimer& gt)
// {
// 
// }
// 
// void D3DApp::UpdateObjectCBs(const GameTimer& gt)
// {
// 	for (auto& e : AllRitems)
// 	{
// 		// Only update the cbuffer data if the constants have changed.  
// 		// This needs to be tracked per frame resource.
// 		if (e->NumFramesDirty > 0)
// 		{
// 			XMMATRIX world = XMLoadFloat4x4(&e->World);
// 			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);
// 
// 			ObjectConstants objConstants;
// 			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
// 			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
// 
// 			ObjectBuffer.Get()->CopyData(e->ObjCBIndex, objConstants);
// 
// 			// Next FrameResource need to be updated too.
// 			e->NumFramesDirty--;
// 		}
// 	}
// }
// 
// void D3DApp::UpdateMaterialCBs(const GameTimer& gt)
// {
// 	for (auto& e : mMaterials)
// 	{
// 		// Only update the cbuffer data if the constants have changed.  If the cbuffer
// 		// data changes, it needs to be updated for each FrameResource.
// 		Material* mat = e.second.get();
// 		if (mat->NumFramesDirty > 0)
// 		{
// 			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);
// 
// 			MaterialConstants matConstants;
// 			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
// 			matConstants.FresnelR0 = mat->FresnelR0;
// 			matConstants.Roughness = mat->Roughness;
// 			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));
// 
// 			MaterialBuffer.Get()->CopyData(mat->MatCBIndex, matConstants);
// 
// 			// Next FrameResource need to be updated too.
// 			mat->NumFramesDirty--;
// 		}
// 	}
// }
// 
// void D3DApp::UpdateMainPassCB(const GameTimer& gt)
// {
// 	XMMATRIX view = XMLoadFloat4x4(&mView);
// 	XMMATRIX proj = XMLoadFloat4x4(&mProj);
// 
// 	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
// 	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
// 	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
// 	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
// 
// 	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
// 	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
// 	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
// 	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
// 	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
// 	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
// 	mMainPassCB.EyePosW = mEyePos;
// 	mMainPassCB.RenderTargetSize = XMFLOAT2((float)ClientWidth, (float)ClientHeight);
// 	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / ClientWidth, 1.0f / ClientHeight);
// 	mMainPassCB.NearZ = 1.0f;
// 	mMainPassCB.FarZ = 1000.0f;
// 	mMainPassCB.TotalTime = gt.TotalTime();
// 	mMainPassCB.DeltaTime = gt.DeltaTime();
// 	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
// 	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
// 	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
// 	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
// 	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
// 	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
// 	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
// 
// 	auto currPassCB = CurFrameResource->PassConstBuffer.get();
// 	currPassCB->CopyData(0, mMainPassCB);
// }

// void D3DApp::LoadTextures()
// {
// 	auto woodCrateTex = std::make_unique<Texture>();
// 	woodCrateTex->Name = "woodCrateTex";
// 	woodCrateTex->Filename = L"../Textures/WoodCrate01.dds";
// 	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3D12Device.Get(),
// 		CommandList.Get(), woodCrateTex->Filename.c_str(),
// 		woodCrateTex->Resource, woodCrateTex->UploadHeap));
// 
// 	mTextures[woodCrateTex->Name] = std::move(woodCrateTex);
// }
// 
// void D3DApp::BuildRootSignature()
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
// 	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&OpaquePipelineStateObject)));
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
// 
// void D3DApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
// {
// 	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(CommandListResource::ObjectConstants));
// 	UINT matCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
// 
// 	auto ObjectConstBuffer = CurFrameResource->ObjectConstBuffer->Resource();
// 	auto matCB = CurFrameResource->MaterialConstBuffer->Resource();
// 
// 	// For each render item...
// 	for (size_t i = 0; i < ritems.size(); ++i)
// 	{
// 		auto ri = ritems[i];
// 
// 		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
// 		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
// 		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);
// 
// 		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(SrvHeap->GetGPUDescriptorHandleForHeapStart());
// 		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize);
// 
// 		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = ObjectConstBuffer->GetGPUVirtualAddress() + ri->ObjCBIndex*objCBByteSize;
// 		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;
// 
// 		cmdList->SetGraphicsRootDescriptorTable(0, tex);
// 		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
// 		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);
// 
// 		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
// 	}
// }

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3DApp::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

