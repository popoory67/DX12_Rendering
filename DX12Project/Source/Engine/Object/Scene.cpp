#include "Scene.h"
#include "PrimitiveComponent.h"
#include "TaskGraph.h"
#include "ThreadBase.h"
#include "RenderThread.h"
#include "RenderPass.h"
#include "CommandList.h"
#include "Entity.h"

Scene::Scene()
{
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

void Scene::AddEntity(std::shared_ptr<Entity> InEntity)
{
	assert(InEntity.use_count() > 0);

	Entities[InEntity->GetId()] = InEntity;
	EntityVisibility[InEntity->GetId()] = false;
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

bool Scene::IsVisible(unsigned InId)
{
	// Visible test

	return false;
}

void Scene::RenderScene()
{
    // TODO : this is test code
    // 1. Mesh pass
    {
        // Create mesh batches
        // Send to render thread with the primitive info
		MeshRenderBatch batch;
        for (const auto& it : Primitives)
        {
            // If the primitive is invisible, this scope is skipped.
            if (it.second == 0)
            {
                continue;
            }

			batch.Elements.emplace_back(std::move(it.first->PrimitiveData));
        }

        // Lambda moves to render thread
        TaskGraphSystem::Get().AddTask<RenderCommand>([meshBatch = std::move(batch)](const RHICommandList& InCommandList) mutable
        {
			MeshRenderPass* meshPass = new MeshRenderPass();
			meshPass->AddMeshBatch(std::move(meshBatch));

			RenderGraph::Get().AddTask(std::move(meshPass));

        }, ThreadType::Render);
    }

}
