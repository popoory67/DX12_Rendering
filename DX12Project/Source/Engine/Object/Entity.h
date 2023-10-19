#pragma once
#include "Class.h"
#include <vector>
#include <memory>

class Entity
{
public:
	Entity() = delete;
	Entity(class Scene* InScene);
	virtual ~Entity() = default;

	virtual void Initialize() {}

	Scene* GetScene() const;

	size_t GetType() { return EntityTypeHash; }
	void SetIndex(size_t InIndex) { Index = InIndex; }
	size_t GetId();

	bool IsEqualTypeId(size_t InTypeId);

	template<typename ComponentType>
	void AddComponent(std::shared_ptr<ComponentType> InCompnent)
	{
		Components.emplace_back(InCompnent);
		AddType(ComponentType::StaticClass());
	}

	template<typename ComponentType>
	void GetComponents(std::vector<class Component*>& OutComponents)
	{
		for (auto pComponent : Components)
		{
			if (pComponent->IsEqualClass(ComponentType::StaticClass()))
			{
				OutComponents.emplace_back(pComponent.get());
			}
		}
	}

	std::vector<std::shared_ptr<class Component>>& GetComponentsAll()
	{
		return Components;
	}

private:

	void AddType(ClassType* InType);
	
private:
	std::vector<ClassType*> EntityTypes;
	std::vector<std::shared_ptr<class Component>> Components;

	Scene* Owner;

	size_t EntityTypeHash;
	size_t Index = 0;
	size_t Id = 0;
};