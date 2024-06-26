#pragma once
#include "Entity.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "MaterialComponent.h"

class CameraEntity : public Entity
{
	using Parent = Entity;

public:
	CameraEntity() = delete;
	CameraEntity(class Scene* InScene);
	virtual ~CameraEntity() = default;

private:
	void UpCamera();
	void DownCamera();
	void LeftCamera();
	void RightCamera();
	void TurnLeftCamera();
	void TurnRightCamera();

	void UpScrollCamera();
	void DownScrollCamera();

	void CameraReset();

private:
	std::shared_ptr<class TransformComponent> OriginalTransform;
	std::shared_ptr<class CameraComponent> CameraController;

	float MoveSpeed = 1.0f;
};

class ModelEntity : public Entity
{
	using Parent = Entity;

public:
	ModelEntity() = delete;
	ModelEntity(class Scene* InScene);
	virtual ~ModelEntity() = default;

	void SetStaticMesh(const std::wstring& InPath);

	void SetPosition(float InX, float InY, float InZ = 0.0f);
	void SetRotation(float InRoll, float InPitch, float InYaw);
	void SetScale(float InX, float InY, float InZ);

private:
	std::shared_ptr<class MeshComponent> StaticMesh;
	std::shared_ptr<class TransformComponent> OriginalTransform;
	std::shared_ptr<class MaterialComponent> Material;
};
