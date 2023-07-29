#pragma once
#include "Entity.h"
#include <typeinfo>
#include <unordered_map>

class EntityQuery
{
public:
	EntityQuery() {}
	~EntityQuery() {}

	void AddEntity(class Entity* InEntity);

	template<typename ComponentType>
	void GetComponents(std::vector<ComponentType*>& OutComponents)
	{
		for (auto it : Entities)
		{
			for (Entity* pEntity : it.second)
			{
				if (pEntity)
				{
					pEntity->GetComponents<ComponentType>(*reinterpret_cast<std::vector<Component*>*>(&OutComponents));
				}
			}
		}
	}

private:
	std::unordered_map<size_t, std::vector<class Entity*>> Entities; // need to check the vector size if it is over 16kb
};