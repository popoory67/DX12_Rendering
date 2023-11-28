#pragma once
#include <d3d12.h>
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Mesh.h"
#include "MeshLoader.h"

class PrimitiveBuilder : public Uncopyable
{
public:
	PrimitiveBuilder() = default;
	virtual ~PrimitiveBuilder();

	virtual void Build(class PrimitiveProxy* InProxy);
	virtual void LoadStaticMesh(const std::wstring& InPath);

protected:
	static MeshLoader& GetLoader();

private:
	static MeshLoader* Loader;
};

class PrimitiveProxy
{
	friend class PrimitiveBuilder;
	friend class Scene;

public:
	PrimitiveProxy() = delete;
	PrimitiveProxy(class PrimitiveComponent* InComponent);
	virtual ~PrimitiveProxy() = default;

protected:
	class PrimitiveComponent* OwnerComponent;
	MeshRenderBatchElement PrimitiveData;
};

class PrimitiveComponent : public RenderComponent
{
	using Parent = RenderComponent;

public:
	PrimitiveComponent() = delete;
	PrimitiveComponent(class Scene* InScene, Component* InParent = nullptr);
	virtual ~PrimitiveComponent();

	void UpdateResources() override;
	virtual PrimitiveProxy* CreateProxy();

	UINT GetIndex() const { return Index; }
	void SetMeshModel(const std::wstring& InAssetName);

protected:
	virtual void CreateResource();

protected:
	static std::unique_ptr<PrimitiveBuilder> Builder;

	PrimitiveProxy* Proxy = nullptr;
	
private:
	UINT Index = 0;
};