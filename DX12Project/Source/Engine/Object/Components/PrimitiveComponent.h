#pragma once
#include <d3d12.h>
#include "RenderComponent.h"
#include "Mesh.h"

class PrimitiveBuilder : public Uncopyable
{
public:
	PrimitiveBuilder() = default;
	virtual ~PrimitiveBuilder() = default;

	virtual void Build(VertexStream& InVertexStream, class PrimitiveProxy* InProxy);
};

class PrimitiveProxy
{
	friend class PrimitiveBuilder;
	friend class Scene;

public:
	PrimitiveProxy() = delete;
	PrimitiveProxy(class PrimitiveComponent* InComponent);
	virtual ~PrimitiveProxy() = default;

	virtual void DrawElements() {}

private:
	class PrimitiveComponent* OwnerComponent;
	VertexStream* PrimitiveData;
};

class PrimitiveComponent : public RenderComponent
{
	using Parent = RenderComponent;

public:
	PrimitiveComponent() = delete;
	PrimitiveComponent(class Scene* InScene, Component* InParent);
	virtual ~PrimitiveComponent();
	
	UINT GetIndex() const { return Index; }

	void SetMeshModel(const std::string& InPath);

	virtual PrimitiveProxy* CreateProxy();

protected:
	virtual void CreateResource();
	
	void Update() override;

public:
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	PrimitiveProxy* Proxy = nullptr;

protected:
	static PrimitiveBuilder* Builder;

private:
	UINT Index = 0;
};