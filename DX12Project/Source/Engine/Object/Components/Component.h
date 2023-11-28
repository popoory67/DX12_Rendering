#pragma once
#include "Class.h"
#include <string>

class Component : public BaseClass
{
	friend class Scene;
	friend class Entity;

public:
	Component() = delete;
	Component(class Scene* InScene, Component* InParent);
	virtual ~Component() = default;

	class Entity* GetOwner() const;

protected:
	virtual void Initialize() {}
	virtual void Update() {}
	virtual class Scene* GetScene() const;

	void Register(class Scene* InScene);
	Component* GetParent() const;

protected:
	std::string Name;

private:
	class Scene* CurrentScene = nullptr;
	class Entity* Owner = nullptr;

	Component* Parent = nullptr;
};