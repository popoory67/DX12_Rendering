#pragma once

#include <memory>
#include <d3d12.h>
#include "D3D12Resource.h"

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

	void Initialize(class D3D12Device* InD3D12Device);
	void RenderScreenView(class D3D12CommandList* InCommandList);
	void RenderPrimitives(class D3D12CommandList* InCommandList);
	void RenderMaterials(class D3D12CommandList* InCommandList);

	void SetCurrentScene(class Scene* InScene);

private:
	void SceneRendering();

private:
	// Render items divided by PSO.
	//std::map<unsigned, class Primitive*> PrimitiveItems;
	//std::map<unsigned, class Material*> MaterialItems;

	// pso 안에 종류별로 잇어야함
	/*
				FD3D12ShaderResourceViewCache SRVCache;
			FD3D12ConstantBufferCache CBVCache;
			FD3D12UnorderedAccessViewCache UAVCache;
			FD3D12SamplerStateCache SamplerCache;
	*/
	std::unique_ptr<class D3D12UploadResource<class Primitive>> ObjectConstBuffer;
	std::unique_ptr<class D3D12UploadResource<class Material>> MaterialConstBuffer;

	D3D12_GPU_VIRTUAL_ADDRESS ConstBufferCache;

	class D3D12RootSignature* RootSignature = nullptr;

	class Scene* CurrentScene = nullptr;

	//ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)
	//std::vector<RenderItem*> RenderItems;

// 	ConstantResource<ObjectConstants>* ObjectBuffer;
// 	ConstantResource<PassConstants>* MainPassBuffer;
// 	ConstantResource<MaterialConstants>* MaterialBuffer;
};