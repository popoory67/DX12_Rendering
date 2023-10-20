#pragma once
#include "D3DUtil.h"
#include <typeindex>
#include <memory>
#include <any>

class ClassTypeBase
{
public:
	size_t GetType() const
	{
		return GUID;
	}

protected:
	size_t GUID;
};

template<class T>
class ClassType : public ClassTypeBase
{
private:
	size_t CreateType()
	{
		std::type_index type(typeid(T));
		return type.hash_code();
	}

public:
	ClassType()
	{
		GUID = CreateType();
	}

	~ClassType() = default;
};

template<class T>
static ClassType<T>& StaticClass()
{
	static ClassType<T> type{};
	return type;
}

class BaseClass
{
public:
	BaseClass() = default;
	virtual ~BaseClass() = default;

	const char* GetClassName() const 
	{
		return typeid(*this).name();
	}

	const size_t GetClass() const
	{
		std::type_index type(typeid(*this));
		return type.hash_code();
	}

	bool IsEqualClass(ClassTypeBase& InClass)
	{
		return InClass.GetType() == GetClass();
	}
};