#pragma once
#include "Util.h"
#include <unordered_map>
#include <memory>

class SceneRenderer : public Uncopyable
{
public:
	SceneRenderer() = default;
	virtual ~SceneRenderer() = default;

	bool Initialize();
	void BeginRender();
	void Render(class RHICommandContext& InContext);
	void EndRender();
};