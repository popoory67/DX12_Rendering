#include "Entity.h"
#include <assert.h>

size_t Entity::Id = 0;

Entity::Entity(Scene* InScene)
	: Owner(InScene)
{

}

size_t Entity::GetId() const
{
	++Id;
	return Id;
}

Scene* Entity::GetScene() const
{
	return Owner;
}