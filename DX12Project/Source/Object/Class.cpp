#include "stdafx.h"
#include "Component.h"
#include "D3DUtil.h"

static size_t CreateType(std::any InClass)
{
	std::type_index type(typeid(InClass));
	return type.hash_code();
}

BClassType::BClassType()
{
}

BClassType::BClassType(std::any InClass)
{
	GUID = CreateType(InClass);
}

BClassType::~BClassType()
{
}

size_t BClassType::GetType()
{
	return GUID;
}

// ----------------------------------------------------------------------------//

std::unique_ptr<BClassType> BClass::Type;

BClass::BClass()
{
	Type = std::make_unique<BClassType>(this);
}

BClass::~BClass()
{

}

BClassType* BClass::StaticClass()
{
	Type = std::make_unique<BClassType>();
	return Type.get();
}

BClassType* BClass::GetClass()
{
	return Type.get();
}

bool BClass::IsEqualClass(BClassType* InClass)
{
	assert(InClass);
	assert(Type);

	if (InClass->GetType() == Type->GetType())
		return true;

	return false;
}