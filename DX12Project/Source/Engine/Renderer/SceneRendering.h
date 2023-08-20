#pragma once
#include <unordered_map>
#include <memory>

class SceneRenderer
{
public:
	SceneRenderer() = default;
	SceneRenderer(const SceneRenderer& rhs) = delete;
	SceneRenderer& operator=(const SceneRenderer& rhs) = delete;

	~SceneRenderer() = default;

	bool Initialize();
	void BeginRender();
	void Render(class RHICommandList& InCommandList);
	void EndRender();
};