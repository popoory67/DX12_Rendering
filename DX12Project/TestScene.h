#pragma once
#include "Scene.h"

class TestScene : public Scene
{
	using Parent = Scene;

public:
	TestScene();
	virtual ~TestScene();

	void Start() override;
	void Update() override;

private:
	std::shared_ptr<class ModelEntity> Wolf;
	//std::shared_ptr<ModelEntity> Diamond;

	std::shared_ptr<class CameraEntity> Camera;
};