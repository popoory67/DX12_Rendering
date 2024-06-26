#include "Scene.h"
#include "ThreadBase.h"
#include "RenderThread.h"
#include "RenderPass.h"
#include "CommandList.h"
#include "CommandContext.h"
#include "Entity.h"
#include "Camera.h"
#include "PrimitiveComponent.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	SafeDeleteVector(Cameras);
}

void Scene::Start()
{
	for (const auto& it : Entities)
	{
		std::shared_ptr<Entity> entity = it.second;
		{
			entity->Initialize();

			auto components = entity->GetComponentsAll();
            for (auto& component : components)
            {
				component->Initialize();

				auto renderComponent = std::dynamic_pointer_cast<RenderComponent>(component);
				if (renderComponent)
				{
					renderComponent->UpdateResources();
				}
            }

			Components.insert(Components.end(), components.begin(), components.end());
		}
	}
}

void Scene::Update()
{
	UpdateComponents();
	// UpdateVisibility();

	UpdateCamera(GetCommandContext());

	// Addressed a collection of entity data to SceneRendering
	RenderScene();
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

	Primitives.emplace(proxy, 1);
}

int Scene::AddNewCamera()
{
	Camera* newCamera = new Camera();

	Cameras.emplace_back(newCamera);

    if (!MainCamera)
    {
        MainCamera = newCamera;
    }

	return newCamera->GetId();
}

void Scene::SetMainCamera(int InCameraId)
{
	if (MainCamera->GetId() == InCameraId)
	{
		return;
	}

	auto camera = std::find_if(Cameras.begin(), Cameras.end(), [InCameraId](const auto& it)
	{
		return it->GetId() == InCameraId;
	});

	if (*camera)
	{
		MainCamera = *camera;
	}
	else
	{
		// error message
	}
}

const Camera* Scene::GetMainCamera()
{
    if (!MainCamera)
    {
		AddNewCamera();
    }
	return MainCamera;
}

class Camera* Scene::GetCamera(int InCameraId)
{
    auto camera = std::find_if(Cameras.begin(), Cameras.end(), [InCameraId](const auto& it)
    {
        return it->GetId() == InCameraId;
    });

    if (!(*camera))
    {
		// error message
    }
	return *camera;
}

void Scene::UpdateVisibility()
{
	// TODO
	// Primitive have to be rendered depending on Visibility flag
	for (auto it : EntityVisibility)
	{
		if (IsVisible(it.first))
		{
			it.second = 1;
		}
	}
}

bool Scene::IsVisible(int InId)
{
	// TODO
	// Visible test

	return false;
}

void Scene::UpdateComponents()
{
	// TODO
	// distribute fine components to each worker threads

	for (const auto& component : Components)
	{
		component->Update();
	}
}

void Scene::UpdateCamera(const RHICommandContext& InContext)
{
	XMMATRIX world = XMMATRIX(g_XMIdentityR0, g_XMIdentityR1, g_XMIdentityR2, g_XMIdentityR3);
	XMMATRIX view = MainCamera->GetViewMatrix();
	XMMATRIX projection = MainCamera->GetProjectionMatrix();

	XMMATRIX worldViewProjection = world * view * projection;

	RHICommand_BeginRender* viewportCommand = RHICommand_BeginRender::Create(std::move(worldViewProjection));
	{
		InContext.AddCommand(std::move(viewportCommand));
	}
}

void Scene::RenderScene()
{
    // 1. Mesh pass
    {
        // Create mesh batches
        // Send to render thread with the primitive info
		MeshRenderBatch batch{};

        for (const auto& it : Primitives)
        {
            // If the primitive is invisible, this scope is skipped.
            if (it.second == 0)
            {
                continue;
            }

			PrimitiveProxy* proxy = it.first;
			batch.AddElement(proxy->PrimitiveData);
        }

		// TODO
		// MeshRenderPass and MeshRenderBatch have to be distributed to several Tasks to utilize tons of vertices.
		std::unique_ptr<MeshRenderPass> meshPass = std::make_unique<MeshRenderPass>();
        {
            meshPass->AddMeshBatch(std::move(batch));

			std::unique_ptr<RenderPass> pass(std::move(meshPass));
            RenderGraph::Get().AddTask(std::move(pass));
        }
    }
}
