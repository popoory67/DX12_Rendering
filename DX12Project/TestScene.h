#pragma once

#include "Scene.h"

//class TransformSystem
//{
//public:
//	void Update(std::shared_ptr<class EntityQuery> InQuery);
//};

class TestScene : public Scene
{
public:
	TestScene();
	virtual ~TestScene();

	virtual void Start();

private:
	std::shared_ptr<Entity> Object;
};