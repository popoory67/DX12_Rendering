#pragma once
#include "Class.h"
#include "Component.h"
#include <vector>
#include <memory>

class Entity
{
public:
	Entity() = delete;
	Entity(class Scene* InScene);
	virtual ~Entity() = default;

	virtual void Initialize() {}

	size_t GetId() const;
	Scene* GetScene() const;

	template<typename ComponentType = Component>
	void AddComponent(std::shared_ptr<ComponentType> InCompnent)
	{
		InCompnent->Owner = this;
		Components.emplace_back(InCompnent);
	}

	template<typename ComponentType = Component>
	void GetComponents(std::vector<std::shared_ptr<ComponentType>>& OutComponents)
	{
		for (const auto& pComponent : Components)
		{
			if (pComponent->IsEqualClass(StaticClass<ComponentType>()))
			{
				OutComponents.emplace_back(std::static_pointer_cast<ComponentType>(pComponent));
			}
		}
	}

	std::vector<std::shared_ptr<class Component>>& GetComponentsAll()
	{
		return Components;
	}

	template<typename ComponentType = Component>
	std::shared_ptr<ComponentType> GetComponent()
	{
		for (auto& pComponent : Components)
		{
			if (pComponent->IsEqualClass(StaticClass<ComponentType>()))
			{
				return std::static_pointer_cast<ComponentType>(pComponent);
			}
		}
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<Component>> Components;

	Scene* Owner;
	static size_t Id;
};