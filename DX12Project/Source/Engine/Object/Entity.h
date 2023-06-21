#pragma once
#include "Class.h"

class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;

	size_t GetType() { return EntityTypeHash; }
	void SetIndex(size_t InIndex) { Index = InIndex; }
	size_t GetId();

	bool IsEqualTypeId(size_t InTypeId);

	template<typename ComponentType>
	void AddComponent(std::shared_ptr<ComponentType> InCompnent)
	{
		if (InCompnent.use_count() > 0)
		{
			Components.emplace_back(InCompnent);
			AddType(ComponentType::StaticClass());
		}
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

//	int UpdateFrameRate = -1;

private:

	void AddType(ClassType* InType);
	
private:

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
//	UINT ObjCBIndex = -1;

	std::vector<ClassType*> EntityTypes;
	std::vector<std::shared_ptr<class Component>> Components;

	size_t EntityTypeHash;
	size_t Index = 0;
	size_t Id = 0;
};