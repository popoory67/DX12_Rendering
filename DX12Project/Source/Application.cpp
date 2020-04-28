#include "stdafx.h"
#include "Application.h"
#include "D3D12Rendering.h"
#include "InputManager.h"

// test
#include "../TestScene.h"

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Init()
{
	D3D12Renderer::GetInstance().Initialize();

	// test scene
	TestScene* test = new TestScene();
	D3D12Renderer::GetInstance().AddScene(test);
	D3D12Renderer::GetInstance().SetCurrentScene(test->GetSceneId());
}

int Application::Run()
{
	//CalculateFrameStats();
	D3D12Renderer::GetInstance().Update(GetTimer());
	D3D12Renderer::GetInstance().Render(GetTimer());
}

GameTimer& Application::GetTimer() const
{
	return *InputManager::GetInstance().GetTimer();
}
