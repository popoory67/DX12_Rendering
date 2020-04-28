#pragma once
#include "MathHelper.h"

using namespace DirectX;

//XMFLOAT4X4& GetWorld() { return TransformData->World; }
//XMFLOAT4X4& GetTextureTransform() { return TransformData->TexTransform; }

//void CopyData(std::shared_ptr<CTransform> InTransform);
//std::shared_ptr<CTransform> TransformData = nullptr;
//void Entity::CopyData(std::shared_ptr<Transform> InTransform)
//{
//	TransformData = InTransform;
//}

class Component
{
public:
	Component();
	virtual ~Component();

private:
	// Unique material name for lookup.
	std::string Name;
};
