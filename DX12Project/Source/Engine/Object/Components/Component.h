#pragma once
#include "Class.h"
#include "Scene.h"

struct UploadData
{

};

class Component : public BaseClass
{
public:
	Component() = default;
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
};