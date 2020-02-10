#pragma once
#include "Object.h"
#include "Mesh.h"
#include "Material.h"

class TestObject
{
public:
	TestObject();
	
private:
	void BuildTestObject();

private:
	Object* ObjectData = nullptr;
	Material* MaterialData = nullptr;
	Primitive* PrimitiveData = nullptr;
};

class TestScene
{
public:
	TestScene();
	virtual ~TestScene();

private:
	TestObject* Test = nullptr;
};