#pragma once

#include "MathHelper.h"
#include "Component.h"

using namespace DirectX;

class TransformComponent : public Component
{
	using Parent = Component;

public:
	TransformComponent() = delete;
	TransformComponent(class Scene* InScene, Component* InParent = nullptr);
	virtual ~TransformComponent() = default;

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;
	XMFLOAT3 GetScale() const;

	void SetRelativeTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale = { 1.0f, 1.0f, 1.0f });
	void SetRelativePosition(float InX, float InY, float InZ = 0.0f);
	void SetRelativeRotation(float InRoll, float InPitch, float InYaw = 0.0f);

	void SetTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale = { 1.0f, 1.0f, 1.0f });
	void SetPosition(float InX, float InY, float InZ = 0.0f);
	void SetRotation(float InRoll, float InPitch, float InYaw);
	void SetScale(float InX, float InY, float InZ);

protected:
	// Update transforms of relative components
	//virtual void UpdateTransforms(); 

protected:
	XMFLOAT3 WorldPosition;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;

	//Component* Child = nullptr;
};