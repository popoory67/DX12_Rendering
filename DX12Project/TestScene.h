#pragma once
#include "Scene.h"

class TestScene : public Scene
{
	using Parent = Scene;

public:
	TestScene();
	virtual ~TestScene();

	virtual void Start();

private:
	void UpCamera();
	void DownCamera();
	void LeftCamera();
	void RightCamera();

	void UpScrollCamera();
	void DownScrollCamera();

	void CameraReset();

private:
	std::shared_ptr<Entity> Object;

	std::shared_ptr<class CameraComponent> CameraController;
	float MoveSpeed = 1.0f;
};