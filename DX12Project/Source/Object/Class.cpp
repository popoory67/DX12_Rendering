#include "stdafx.h"
#include "Component.h"
#include "D3DUtil.h"

static size_t CreateType(std::any InClass)
{
	std::type_index type(typeid(InClass));
	return type.hash_code();
}

ClassType::ClassType()
{
}

ClassType::ClassType(std::any InClass)
{
	HashCode = CreateType(InClass);
}

ClassType::~ClassType()
{
}

size_t ClassType::GetType()
{
	return HashCode;
}

// ----------------------------------------------------------------------------//

std::unique_ptr<ClassType> Class::Type;

Class::Class()
{
	Type = std::make_unique<ClassType>(this);
}

Class::~Class()
{

}

ClassType* Class::StaticClass()
{
	Type = std::make_unique<ClassType>();
	return Type.get();
}

ClassType* Class::GetClass()
{
	return Type.get();
}

bool Class::IsEqualClass(ClassType* InClass)
{
	assert(InClass);
	assert(Type);

	if (InClass->GetType() == Type->GetType())
		return true;

	return false;
}