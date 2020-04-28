#pragma once

class Entity
{
public:
	Entity() = default;

	size_t GetType() { return TypeId; }
	void SetIndex(unsigned InIndex) { Index = InIndex; }
	size_t GetId();

	bool IsEqualTypeId(size_t InTypeId);

	template<typename ComponentType>
	void AddComponent(class ComponentType* InCompnent)
	{
		if (InCompnent)
		{
			Components.emplace_back(InCompnent);
			AddType(typeid(ComponentType));
		}
	}

	template<typename ComponentType>
	void AddComponent(std::shared_ptr<class ComponentType> InCompnent)
	{
		if (InCompnent.use_count() > 0)
		{
			Components.emplace_back(InCompnent.get());
			AddType(typeid(ComponentType));
		}
	}

	template<typename ComponentType>
	void GetComponents(std::vector<ComponentType*> OutComponents)
	{
		for (auto pComponent : Components)
		{
			if (pComponent && typeid(pComponent) == typeid(ComponentType))
			{
				OutComponents.emplace_back(pComponent);
			}
		}
	}

//	int UpdateFrameRate = -1;

private:

	void AddType(const type_info& InType);
	
private:

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
//	UINT ObjCBIndex = -1;

	std::vector<type_info> Types;
	std::vector<class Component*> Components;

	size_t TypeId;
	unsigned Index = 0;
	size_t Id = 0;
};