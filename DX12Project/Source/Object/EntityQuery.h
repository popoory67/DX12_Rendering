#pragma once
#include "Entity.h"
#include <typeinfo>

class EntityQuery
{
public:
	EntityQuery() {}
	~EntityQuery() {}

	void AddEntity(class Entity* InEntity);

	template<typename ComponentType>
	void GetComponents(std::vector<ComponentType*> OutComponents)
	{
		//for (auto entityList : Entities)
		//{

		//	if (pEntity)
		//	{
		//		pEntity->GetComponents<ComponentType>(OutComponents);
		//	}
		//}
	}

private:
	std::map<size_t, std::vector<class Entity*>> Entities; // need to check the vector size if it is over 16kb
};