#pragma once

class BClassType
{
public:
	BClassType();
	BClassType(std::any InClass);
	~BClassType();

	size_t GetType();

private:
	size_t GUID;
};

class BClass
{
public:
	BClass();
	virtual ~BClass();

	static BClassType* StaticClass();

	BClassType* GetClass();
	bool IsEqualClass(BClassType* InClass);

private:

protected:
	static std::unique_ptr<BClassType> Type;
};