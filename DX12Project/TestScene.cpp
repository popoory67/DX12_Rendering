#include "TestScene.h"
#include "Entity.h"
#include "PrimitiveComponent.h"

TestScene::TestScene()
{

}

TestScene::~TestScene()
{

}

void TestScene::Start()
{
	Object = std::make_shared<Entity>();
	if (Object)
	{
		std::shared_ptr<PrimitiveComponent> PrimitiveData = std::make_shared<PrimitiveComponent>(this, nullptr);
		PrimitiveData->SetMeshModel({});
		Object->AddComponent(PrimitiveData);

		AddEntity(Object);
	}
}