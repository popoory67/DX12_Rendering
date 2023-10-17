#pragma once

#include "MathHelper.h"
#include "Component.h"

using namespace DirectX;

class TransformComponent
{
public:
	TransformComponent() = delete;
	TransformComponent(Component* InChild);
	virtual ~TransformComponent() = default;

	void SetRelativeTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale = { 1.0f, 1.0f, 1.0f });
	void SetRelativePosition(float InX, float InY, float InZ);


	void SetTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale = { 1.0f, 1.0f, 1.0f });
	void SetPosition(float InX, float InY, float InZ);
	void SetRotation(float InRoll, float InPitch, float InYaw);
	void SetScale(float InX, float InY, float InZ);

protected:
	// Update transforms of relative components
	//virtual void UpdateTransforms(); 

protected:
	XMFLOAT3 WorldPosition;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;

	Component* Child = nullptr;
};