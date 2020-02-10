#pragma once
#include "MathHelper.h"

using namespace DirectX;

// Data
struct Transform
{
	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

// 일단 데이터 빼고 렌더관련 아이템 전부다 넣어봄
class Component
{
public:
	Component();
	virtual ~Component();
};

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
class Object
{
public:
	Object() = default;

	XMFLOAT4X4& GetWorld() { return TransformData->World; }
	XMFLOAT4X4& GetTextureTransform() { return TransformData->TexTransform; }

private:
	int UpdateFrameRate = 0;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Transform* TransformData = nullptr;
};