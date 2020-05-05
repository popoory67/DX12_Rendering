#include "stdafx.h"
#include "Entity.h"
#include "Component.h"

void Entity::AddType(ClassType* InType)
{
	assert(InType);

	auto it = std::find_if(EntityTypes.cbegin(), EntityTypes.cend(), [InType](ClassType* pType)
	{
		if (pType->GetType() == InType->GetType())
			return true;
		return false;
	});

	if (it == EntityTypes.cend())
	{
		// test hash
		EntityTypeHash <<= 2;
		EntityTypeHash |= InType->GetType() >> 62;

		EntityTypes.emplace_back(InType);
	}
}

size_t Entity::GetId()
{
	return EntityTypeHash & Index;
}

bool Entity::IsEqualTypeId(size_t InTypeId)
{
	return EntityTypeHash != 0 && EntityTypeHash == InTypeId;
}