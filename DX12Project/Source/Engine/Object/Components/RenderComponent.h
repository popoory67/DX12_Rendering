#pragma once
#include "TransformComponent.h"
#include <windows.h>
#include <unordered_map>
#include <any>

class RenderComponent : public TransformComponent
{
	using Parent = TransformComponent;

public:
	RenderComponent() = delete;
	RenderComponent(class Scene* InScene, Component* InParent);
	virtual ~RenderComponent() = default;

	virtual void UpdateResources() {}

protected:
	bool bDirty = false;
};