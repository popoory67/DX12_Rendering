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

private:
	class PrimitiveComponent* OwnerComponent;
	MeshRenderBatchElement PrimitiveData;
};

class PrimitiveComponent 
	: public RenderComponent
	, public TransformComponent // test
{
	using Parent = RenderComponent;

public:
	PrimitiveComponent() = delete;
	PrimitiveComponent(class Scene* InScene, Component* InParent);
	virtual ~PrimitiveComponent();
	
	UINT GetIndex() const { return Index; }

	void SetMeshModel(const std::wstring& InPath);

	virtual PrimitiveProxy* CreateProxy();

protected:
	virtual void CreateResource();
	
	void Update() override;

public:
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
protected:
	static std::unique_ptr<PrimitiveBuilder> Builder;

	PrimitiveProxy* Proxy = nullptr;

private:
	UINT Index = 0;
};