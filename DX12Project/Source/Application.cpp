#include "stdafx.h"
#include "Application.h"
#include "D3D12Rendering.h"
#include "InputManager.h"

// test
#include "../TestScene.h"

HWND Application::MainWindowHandle = nullptr;

Application::Application(HINSTANCE hInstance)
	: ApplicationInstanceHandle(hInstance)
{

}

Application::~Application()
{
}

HWND Application::GetWindowHandle()
{
	return MainWindowHandle;
}

bool Application::Init()
{
	if (D3D12Renderer::GetInstance().Initialize())
	{
		// test scene
		TestScene* test = new TestScene();
		D3D12Renderer::GetInstance().AddScene(test);
		D3D12Renderer::GetInstance().SetCurrentScene(test->GetSceneId());

		return true;
	}

	return false;
}

int Application::Run()
{
	//CalculateFrameStats();
	D3D12Renderer::GetInstance().Update(GetTimer());
	D3D12Renderer::GetInstance().Render(GetTimer());

	return 1;
}

GameTimer& Application::GetTimer() const
{
	return *InputManager::GetInstance().GetTimer();
}
