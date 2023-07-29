#pragma once
#include "RenderComponent.h"

class MaterialComponent : public RenderComponent
{
public:
	MaterialComponent();
	virtual ~MaterialComponent();

	int GetDiffuseSrvHeapIndex() const { return DiffuseSrvHeapIndex; }
	unsigned GetIndex() const { return Index; }

private:
	// Index into SRV heap for diffuse texture.
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;

	// Index into constant buffer corresponding to this material.
	unsigned int Index = 0;
};