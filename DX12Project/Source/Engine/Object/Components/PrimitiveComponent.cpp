#include "PrimitiveComponent.h"
#include "RenderInterface.h"
#include "Scene.h"

MeshLoader* PrimitiveBuilder::Loader = nullptr;

PrimitiveBuilder::~PrimitiveBuilder()
{
	SafeDelete(Loader);
}

void PrimitiveBuilder::Build(PrimitiveProxy* InProxy)
{
    if (!InProxy)
    {
		return;
    }

	VertexStream vertices;
	GetLoader().GetVertices(vertices);

	IndexStream indices;
    GetLoader().GetIndices(indices);

    // To transform vertices on GPU is effective when it has to be processed on vertex shader every frame because of SIMD structure of GPU.
    // If vertices need to be transformed only once, it's better to pre-compute transforms on CPU time.
	{
		// TODO
		// To easily use functions, add a GetTransform function on Entity class.
		// Additionally, the way to process XMFLOAT3 must be simply utilized.
		Entity* entity = InProxy->OwnerComponent->GetOwner();

		std::vector<std::shared_ptr<TransformComponent>> components;
		entity->GetComponents<TransformComponent>(components);

		std::shared_ptr<TransformComponent> transform = components[0];
		{
			for (auto& it : vertices)
			{
				XMFLOAT3 originalPosition = transform->GetPosition();
				{
					it.Position.x += originalPosition.x;
					it.Position.y += originalPosition.y;
					it.Position.z += originalPosition.z;
				}

                XMFLOAT3 originalScale = transform->GetScale();
				{
					it.Position.x *= originalScale.x;
					it.Position.y *= originalScale.y;
					it.Position.z *= originalScale.z;
				}

				XMFLOAT3 originalRotation = transform->GetRotation();
				{
                    XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(originalRotation.x, originalRotation.y, originalRotation.z);

                    XMVECTOR vertexVector = XMLoadFloat3(&it.Position);
                    XMVECTOR rotatedVector = XMVector3Rotate(vertexVector, quaternion);

                    XMStoreFloat3(&it.Position, rotatedVector);
				}
			}
		}
	}

    InProxy->PrimitiveData.Vertices = std::move(vertices);
	InProxy->PrimitiveData.Stride = sizeof(vertices[0]);
    InProxy->PrimitiveData.Indices = std::move(indices);
}

void PrimitiveBuilder::LoadStaticMesh(const std::wstring& InPath)
{
	GetLoader().LoadObj(InPath.c_str());
}

MeshLoader& PrimitiveBuilder::GetLoader()
{
	if (!Loader)
	{
		Loader = MeshLoader::Create();
	}
	return *Loader;
}

PrimitiveProxy::PrimitiveProxy(PrimitiveComponent* InComponent)
	: OwnerComponent(InComponent)
{

}

std::unique_ptr<PrimitiveBuilder> PrimitiveComponent::Builder;

PrimitiveComponent::PrimitiveComponent(Scene* InScene, Component* InParent)
	: Parent(InScene, InParent)
{

}

PrimitiveComponent::~PrimitiveComponent()
{
	SafeDelete(Proxy);
}

void PrimitiveComponent::SetMeshModel(const std::wstring& InPath)
{
	if (!Builder)
	{
		Builder = std::make_unique<PrimitiveBuilder>();
	}

	CreateResource();

	Builder->LoadStaticMesh(InPath);
	Builder->Build(Proxy);
}

PrimitiveProxy* PrimitiveComponent::CreateProxy()
{
	PrimitiveProxy* proxy = new PrimitiveProxy(this);
	assert(proxy);

	Proxy = proxy;

	return proxy;
}

void PrimitiveComponent::CreateResource()
{
	GetScene()->AddPrimitive(this);
}

void PrimitiveComponent::Update()
{

}