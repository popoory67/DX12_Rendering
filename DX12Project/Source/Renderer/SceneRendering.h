#pragma once
#include "D3D12Commands.h"

class SceneRenderer
{
public:
	SceneRenderer() = default;
	~SceneRenderer() = default;

	void RenderScreenView(class D3D12CommandList& InCommandList);
	void RenderPrimitives(class D3D12CommandList& InCommandList);
	void RenderMaterials(class D3D12CommandList& InCommandList);

	std::shared_ptr<class Scene> GetCurrentScene();
	void AddScene(class Scene* InScene);
	void SetCurrentScene(int InIndex);

private:
	void SceneRendering();

private:
	class D3D12RootSignature* RootSignature = nullptr;

	std::map<unsigned int, std::shared_ptr<class Scene>> SceneList;
	unsigned IndexCount = 0;
	int CurrentSceneIndex = -1;

	std::shared_ptr<class Scene> CurrentScene;
	std::vector<std::weak_ptr<class BObjectCommand>> Commands;

	//ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)
};