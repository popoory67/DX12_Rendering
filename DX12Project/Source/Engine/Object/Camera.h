#pragma once
#include "CameraComponent.h"

class Camera
{
	friend class CameraComponent;

public:
	Camera();
	~Camera() = default;

	void Initialize();

	int GetId() const;
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	void SetFOV(float InFOV);
	void SetAspectRatio(float InAspectRatio);
	void SetPlane(float InNear, float InFar);

private:
	void SetTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation);

	void UpdateView();

private:
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMFLOAT3 Position;
	XMFLOAT3 ViewVector;
	XMFLOAT3 UpVector;

	float FOV;
	float AspectRatio;
	float NearPlane;
	float FarPlane;

	static int Id;
};