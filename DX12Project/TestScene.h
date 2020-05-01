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
	virtual void Update();
	virtual void End();

private:
	void BuildRenderItems();

private:
	std::shared_ptr<class TransformSystem> TestTransform;

	// Render items
	std::shared_ptr<class MaterialComponent> MaterialData;
	std::shared_ptr<class PrimitiveComponent> PrimitiveData;
};