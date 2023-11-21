#pragma once
#include "PrimitiveComponent.h"
#include "MaterialComponent.h"

class MeshProxy : public PrimitiveProxy
{
	using Parent = PrimitiveProxy;

	friend class MeshComponent;

public:
	MeshProxy() = delete;
	MeshProxy(class MeshComponent* InComponent);
	virtual ~MeshProxy() = default;
};

class MeshComponent : public PrimitiveComponent
{
	using Parent = PrimitiveComponent;

public:
	MeshComponent() = delete;
	MeshComponent(class Scene* InScene, Component* InParent = nullptr);
	virtual ~MeshComponent();

	PrimitiveProxy* CreateProxy() override;

	void SetMaterialComponent(std::shared_ptr<class MaterialComponent> InMaterialComponent);

private:
	std::shared_ptr<class MaterialComponent> MeshMaterial;
};