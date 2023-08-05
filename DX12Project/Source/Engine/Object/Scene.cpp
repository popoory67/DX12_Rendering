#include "Scene.h"
#include "PrimitiveComponent.h"
#include "TaskGraph.h"
#include "ThreadBase.h"
#include "RenderThread.h"

Scene::Scene()
{
}

Scene::~Scene()
{

}

void Scene::Start()
{
    EntityInterface.reset(new EntityQuery());
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

void Scene::AddPrimitive(PrimitiveComponent* InPrimitiveComponent)
{
	PrimitiveProxy* proxy = InPrimitiveComponent->CreateProxy();
	if (!proxy)
	{
		return;
	}

	InPrimitiveComponent->Proxy = proxy;

	Primitives.emplace(proxy, 1);
}

void Scene::UpdateVisibility()
{
	// visible 여부 확인하지 않고 그리게 수정
	for (auto it : EntityVisibility)
	{
		if (IsVisible(it.first))
		{
			it.second = 1;
		}
	}
}

void Scene::RenderScene()
{
	TaskGraphSystem::Get().AddTask<RenderCommand>(std::move([](const RHICommandList& InCommandList)
	{

	}), ThreadType::Render);

	// create mesh batches
	

    // send to render thread with primitive info
	// lambda to render thread
}

bool Scene::IsVisible(unsigned InId)
{
	// Visible test

	return false;
}
