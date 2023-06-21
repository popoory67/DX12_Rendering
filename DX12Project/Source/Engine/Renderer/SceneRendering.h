#pragma once
#include "D3D12Commands.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12RootSignature.h"
#include "D3D12RenderInterface.h"

class SceneRenderer
{
public:
	SceneRenderer() = delete;
	SceneRenderer(std::shared_ptr<RHI> InRenderInterface);
	SceneRenderer(const SceneRenderer& rhs) = delete;
	SceneRenderer& operator=(const SceneRenderer& rhs) = delete;

	~SceneRenderer() = default;

	std::shared_ptr<class Scene> GetCurrentScene();
	void AddScene(class Scene* InScene);
	void SetCurrentScene(int InIndex);

	bool Initialize();
	void Update(GameTimer& InTimer)/* = 0*/; // update datas
	void BeginRender();
	void Render(GameTimer& InTimer)/* = 0*/; // update render commands
	void EndRender();

private:

	void RenderPrimitives(class D3D12CommandList& InCommandList);
	void RenderMaterials(class D3D12CommandList& InCommandList);

private:
	std::map<unsigned int, std::shared_ptr<class Scene>> SceneList;
	unsigned IndexCount = 0;
	int CurrentSceneIndex = -1;

	std::shared_ptr<class Scene> CurrentScene;
	std::vector<std::shared_ptr<class ObjectCommand>> Commands;

	//ID3D12Resource* Pass = nullptr; // The mean of pass is like a buffer (upload buffer)

	std::shared_ptr<D3D12RHI> RenderInterface;
	//std::unique_ptr<D3D12Viewport> Viewport; // TODO : change unique param to array
};