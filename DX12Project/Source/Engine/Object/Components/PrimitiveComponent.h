#pragma once
#include <d3d12.h>
#include "RenderComponent.h"
#include "Util.h"
#include "Mesh.h"

class PrimitiveBuilder : public Uncopyable
{
public:
	PrimitiveBuilder() = default;
	virtual ~PrimitiveBuilder() = default;

	virtual void Build(const VertexStream& InVertexStream, class PrimitiveProxy* InProxy);
};

class PrimitiveProxy
{
public:
	PrimitiveProxy() = delete;
	PrimitiveProxy(PrimitiveComponent* InComponent);
	virtual ~PrimitiveProxy() = default;

	virtual void DrawElements();

private:
	class PrimitiveComponent* Component;
};

class PrimitiveComponent : public RenderComponent
{
public:
	PrimitiveComponent();
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
	PrimitiveBuilder* Builder = nullptr;

private:
	UINT Index = 0;
};