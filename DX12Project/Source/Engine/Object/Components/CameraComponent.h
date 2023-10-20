#pragma once
#include "Component.h"
#include "TransformComponent.h"

// to camera entity
class CameraComponent : public TransformComponent
{
	using Parent = TransformComponent;

public:
	CameraComponent() = delete;
	CameraComponent(class Scene* InScene, Component* InParent = nullptr);
	virtual ~CameraComponent() = default;

	void SetFOV(float InFOV);
	void SetAspectRatio(float InAspectRatio);
	void SetAspectRatio(float InWidth, float InHeight); // Depending on screen size

protected:
	void Update() override;

private:
	float FOV;
	float AspectRatio;

	int CameraId = -1;
};