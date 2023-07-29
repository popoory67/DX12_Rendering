#include "EntityQuery.h"
#include <assert.h>

void EntityQuery::AddEntity(Entity* InEntity)
{
	assert(InEntity);

	auto it = Entities.find(InEntity->GetType());
	if (it != Entities.cend())
	{
		InEntity->SetIndex(it->second.size());
		it->second.emplace_back(InEntity);
	}
	else
	{
		std::vector<Entity*> entites;
		InEntity->SetIndex(0);
		entites.emplace_back(InEntity);

		Entities.emplace(std::make_pair(InEntity->GetType(), entites));
	}
}