#pragma once
#include "D3D12Rendering.h"
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

	template<typename ComponentType>
	void GetComponents(std::vector<ComponentType*> OutComponents)
	{
		EntityInterface->GetComponents<ComponentType>(OutComponents);
	}

	static D3D12Renderer& GetInstance();

private:
	void UpdateVisibility();
	bool IsVisible(unsigned InId);

protected:
	unsigned int Id = 0;
	std::shared_ptr<class EntityQuery> EntityInterface;

private:
	std::map<size_t, bool> EntityVisibility;
};