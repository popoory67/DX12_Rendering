#pragma once
#include <memory>
#include <any>

class ClassType
{
public:
	ClassType();
	ClassType(std::any InClass);
	~ClassType();

	size_t GetType();

private:
	size_t GUID;
};

class BaseClass
{
public:
	BaseClass();
	virtual ~BaseClass();

	static ClassType* StaticClass();

	ClassType* GetClass();
	bool IsEqualClass(ClassType* InClass);

private:

protected:
	static std::unique_ptr<ClassType> Type;
};