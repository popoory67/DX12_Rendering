#pragma once

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

	void Initialize();
	void RenderScreenView(class D3D12CommandList* InCommandList);
	void RenderPrimitives(class D3D12CommandList* InCommandList);
	void RenderMaterials(class D3D12CommandList* InCommandList);

	std::shared_ptr<class Scene> GetCurrentScene();
	void AddScene(class Scene* InScene);
	void SetCurrentScene(int InIndex);

private:
	template<typename Type>
	void UpdateConstBuffers(D3D12CommandList* InCommandList, enum class RenderType InRenderType, UINT InCount, UINT InSize)
	{
		std::unique_ptr<D3D12UploadResource<Type>> constBuffer = std::make_unique<D3D12UploadResource<Type>>(D3D12Renderer::GetDevice(), InCount, true);

		for (UINT index = 0; index < InCount; ++index)
		{
			D3D12_GPU_VIRTUAL_ADDRESS address = constBuffer->Resource()->GetGPUVirtualAddress() + index * InSize;

			InCommandList->BindConstBuffer(InRenderType, address);
		}
	}

	void SceneRendering();

private:
	class D3D12RootSignature* RootSignature = nullptr;

	std::map<unsigned int, std::shared_ptr<class Scene>> SceneList;
	unsigned IndexCount = 0;
	int CurrentSceneIndex = -1;

	std::shared_ptr<class Scene> CurrentScene;

	//ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)
};