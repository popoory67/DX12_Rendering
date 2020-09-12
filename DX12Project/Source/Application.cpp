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
	// Create Scene
	
	if (D3D12Renderer::Get().Initialize())
	{
		// TODO : SceneRenderer�� �Űܾ���
		// test scene
		TestScene* test = new TestScene();
		D3D12Renderer::Get().AddScene(test);
		D3D12Renderer::Get().SetCurrentScene(test->GetSceneId());

		return true;
	}
	return false;
}

int Application::Run()
{
	// Scene
	auto scene = GetScene();
	// Object Upload Data/Commands ����
	// SceneRendering ó��
	// D3D12 ó��
	
	//CalculateFrameStats();
	
	D3D12Renderer::Get().Update(GetTimer());
	D3D12Renderer::Get().Render(GetTimer());

	return 1;
}

GameTimer& Application::GetTimer() const
{
	return *InputManager::Get().GetTimer();
}

Scene& Application::GetScene()
{
	
}
