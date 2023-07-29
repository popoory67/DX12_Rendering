#pragma once
#include <bitset>
#include <memory>
#include <unordered_map>
#include "EntityQuery.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	virtual void Update();
	virtual void End();

	unsigned GetSceneId() { return Id; }
	void SetSceneId(unsigned InId) { Id = InId; }

	void AddEntity(class Entity* InEntity);
	void AddPrimitive(class PrimitiveComponent* InPrimitiveComponent);

	template<typename ComponentType>
	void GetComponents(std::vector<ComponentType*>& OutComponents)
	{
		EntityInterface->GetComponents<ComponentType>(OutComponents);
	}

private:
	void UpdateVisibility();
	void RenderScene();

	bool IsVisible(unsigned InId);

private:
	unsigned int Id = 0;

	std::unordered_map<size_t, std::bitset<1>> EntityVisibility;
	std::shared_ptr<EntityQuery> EntityInterface;

	std::unordered_map<class PrimitiveProxy*, std::bitset<1>> Primitives;
};