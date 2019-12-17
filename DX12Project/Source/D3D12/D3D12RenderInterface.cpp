#include "stdafx.h"
//#include <d3d12.h>
#include "D3D12RenderInterface.h"
#include "D3D12Device.h"
#include "D3D12Commands.h"
#include "D3D12Descriptor.h"
#include "D3D12SwapChain.h"
#include "D3D12Resource.h"
#include "D3D12PipelineState.h"

D3D12RenderInterface::D3D12RenderInterface(D3D12Device* InDevice, D3D12CommandList* InCommandList)
{
	assert(InDevice);
	assert(InCommandList);

	Device = InDevice;

	CmdListExecutor = new D3D12CommandListExecutor(Device);

	SwapChain = new D3D12SwapChain(Device);
	if (SwapChain)
	{
		SwapChain->Create(Device, CmdListExecutor);
	}

	DepthStencilBuffer = new D3D12DepthStencilResource(Device, InCommandList);

	// test
	D3D12PipelineState* pPipelineState = new D3D12PipelineState(InDevice, GetBackBufferFormat(), GetDepthStencilFormat());
	if (pPipelineState)
	{
		PipelineStates.emplace_back(pPipelineState);
	}
}

D3D12RenderInterface::~D3D12RenderInterface()
{
	delete(Device);
}

DXGI_FORMAT D3D12RenderInterface::GetBackBufferFormat() const
{ 
	assert(SwapChain); 
	return SwapChain->GetBackBufferFormat(); 
}

DXGI_FORMAT D3D12RenderInterface::GetDepthStencilFormat() const
{ 
	assert(DepthStencilBuffer); 
	return DepthStencilBuffer->GetDepthStencilFormat(); 
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
	for (D3D12PipelineState* pPso : PipelineStates)
	{
		InCommandList->SetPipelineState(pPso);
		// render
	}

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

/*

void FD3D12CommandContext::RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
{
	CommitGraphicsResourceTables();
	CommitNonComputeShaderConstants();

	uint32 VertexCount = GetVertexCountForPrimitiveCount(NumPrimitives, StateCache.GetGraphicsPipelinePrimitiveType());

	NumInstances = FMath::Max<uint32>(1, NumInstances);
	numDraws++;
	numPrimitives += NumInstances * NumPrimitives;
	if (bTrackingEvents)
	{
		GetParentDevice()->RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
	}

	StateCache.ApplyState<D3D12PT_Graphics>(); // RSSetViewports 이런애들 들어가있는거
	CommandListHandle->DrawInstanced(VertexCount, NumInstances, BaseVertexIndex, 0);

#if UE_BUILD_DEBUG
	OwningRHI.DrawCount++;
#endif
	DEBUG_EXECUTE_COMMAND_LIST(this);
}

파라미터로 commandlist를 넣어서 처리하도록 함
굳이 불러와서 commmandlist에 넣어주는게 아니라
*/
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

///////////////////////////////

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