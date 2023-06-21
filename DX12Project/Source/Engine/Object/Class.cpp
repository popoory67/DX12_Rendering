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
	GUID = CreateType(InClass);
}

ClassType::~ClassType()
{
}

size_t ClassType::GetType()
{
	return GUID;
}

// ----------------------------------------------------------------------------//

std::unique_ptr<ClassType> BaseClass::Type;

BaseClass::BaseClass()
{
	Type = std::make_unique<ClassType>(this);
}

BaseClass::~BaseClass()
{

}

ClassType* BaseClass::StaticClass()
{
	Type = std::make_unique<ClassType>();
	return Type.get();
}

ClassType* BaseClass::GetClass()
{
	return Type.get();
}

bool BaseClass::IsEqualClass(ClassType* InClass)
{
	assert(InClass);
	assert(Type);

	if (InClass->GetType() == Type->GetType())
		return true;

	return false;
}