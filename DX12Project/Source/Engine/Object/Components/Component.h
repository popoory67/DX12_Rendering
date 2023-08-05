#pragma once
#include "Class.h"
#include <string>

class Component : public BaseClass
{
public:
	Component() = delete;
	Component(class Scene* InScene, Component* InParent);
	virtual ~Component() = default;

protected:
	virtual void Update() {}
	virtual class Scene* GetScene() const;

	void Register(class Scene* InScene);
	Component* GetParent() const;

protected:
	std::string Name;

	int NumFramesDirty = 3;

private:
	class Scene* CurrentScene = nullptr;
	class Component* Parent = nullptr;
};