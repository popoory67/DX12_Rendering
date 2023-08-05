#pragma once
#include <unordered_map>
#include <memory>

class SceneRenderer
{
public:
	SceneRenderer() = delete;
	SceneRenderer(std::shared_ptr<class RHI> InRenderInterface);
	SceneRenderer(const SceneRenderer& rhs) = delete;
	SceneRenderer& operator=(const SceneRenderer& rhs) = delete;

	~SceneRenderer() = default;

	std::shared_ptr<class Scene> GetCurrentScene();
	void AddScene(class Scene* InScene);
	void SetCurrentScene(int InIndex);

	bool Initialize();
	void BeginRender();
	void Render(class RHICommandList& InCommandList);
	void EndRender();

private:

private:
	std::unordered_map<unsigned int, std::shared_ptr<class Scene>> SceneList;
	unsigned IndexCount = 0;
	int CurrentSceneIndex = -1;

	std::shared_ptr<class Scene> CurrentScene;

	std::shared_ptr<class D3D12RHI> RenderInterface;
	//std::unique_ptr<D3D12Viewport> Viewport; // TODO : change unique param to array
};