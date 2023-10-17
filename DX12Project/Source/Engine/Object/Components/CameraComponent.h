#pragma once
#include "Component.h"
#include "TransformComponent.h"

// to camera entity
class CameraComponent : public TransformComponent, public Component
{
	using Parent = Component;

public:
	CameraComponent() = delete;
	CameraComponent(class Scene* InScene, Component* InParent);
	virtual ~CameraComponent() = default;

	void SetFOV(float InFOV);
	void SetAspectRatio(float InAspectRatio);
	void SetAspectRatio(float InWidth, float InHeight); // Depending on screen size

protected:
	void Update() override;

private:
	float MoveSpeed = 1.0f;
	float FOV;
	float AspectRatio;

	int CameraId = -1;
};