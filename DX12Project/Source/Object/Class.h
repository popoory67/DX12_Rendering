#pragma once

class ClassType
{
public:
	ClassType();
	ClassType(std::any InClass);
	~ClassType();

	size_t GetType();

private:
	size_t HashCode;
};

class Class
{
public:
	Class();
	virtual ~Class();

	static ClassType* StaticClass();

	ClassType* GetClass();
	bool IsEqualClass(ClassType* InClass);

private:

protected:
	static std::unique_ptr<ClassType> Type;
};