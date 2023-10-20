#pragma once
#include "Scene.h"

class Model : public Entity
{
	using Parent = Entity;

public:
	Model() = delete;
	Model(class Scene* InScene);
	virtual ~Model() = default;

	void Initialize() override;

private:
	std::shared_ptr<class PrimitiveComponent> StaticMesh;
	std::shared_ptr<class TransformComponent> OriginalTransform;
};

class TestScene : public Scene
{
	using Parent = Scene;

public:
	TestScene();
	virtual ~TestScene();

	void Start() override;
	void Update() override;

private:
	void UpCamera();
	void DownCamera();
	void LeftCamera();
	void RightCamera();

	void UpScrollCamera();
	void DownScrollCamera();

	void CameraReset();

private:
	std::shared_ptr<Model> Object;

	std::shared_ptr<class CameraComponent> CameraController;
	float MoveSpeed = 1.0f;
};