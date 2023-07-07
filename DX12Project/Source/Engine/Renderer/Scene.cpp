#include "stdafx.h"
#include "Scene.h"

Scene::Scene()
{
	EntityInterface.reset(new EntityQuery());
}

Scene::~Scene()
{

}

void Scene::Start()
{
}

void Scene::Update()
{
}

void Scene::End()
{
}

void Scene::AddEntity(Entity* InEntity)
{
	EntityInterface->AddEntity(InEntity);

	auto it = EntityVisibility.find(InEntity->GetId());
	if (it == EntityVisibility.cend())
	{
		EntityVisibility.emplace(std::make_pair(InEntity->GetId(), false));
	}
}

void Scene::UpdateVisibility()
{
	// visible 여부 확인하지 않고 그리게 수정
	for (auto it : EntityVisibility)
	{
		if (IsVisible(it.first))
		{
			it.second = true;
		}
	}
}

bool Scene::IsVisible(unsigned InId)
{
	// Visible test

	return false;
}
