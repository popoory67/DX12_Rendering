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

	bool Initialize();
	void BeginRender();
	void Render(class RHICommandList& InCommandList);
	void EndRender();

private:
	std::shared_ptr<class D3D12RHI> RenderInterface;
	//std::unique_ptr<D3D12Viewport> Viewport; // TODO : change unique param to array
};