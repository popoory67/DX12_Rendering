#include "stdafx.h"
#include "Application.h"
#include "InputManager.h"
#include "CommandList.h"
#include "SceneRendering.h"
#include "Viewport.h"

// dummy
#include "../TestScene.h"

Application::Application()
	: Caption(L"Application")
	, ClientWidth(800)
	, ClientHeight(600)
{

}

Application::~Application()
{
}

bool Application::Initialize(std::shared_ptr<RHI> InRenderInterface)
{
	InRenderInterface->Initialize();

	// create viewport
	if (RHIViewport* viewport = InRenderInterface->CreateViewport(MainWindowHandle, ClientWidth, ClientHeight))
	{
		ViewportRenderer = std::make_unique<Viewport>(std::move(viewport));
	}

	// create scene renderer
	Renderer = std::make_unique<SceneRenderer>(InRenderInterface);
	if (Renderer)
	{
		if (Renderer->Initialize())
		{
			// Create dummy scene
			TestScene* test = new TestScene();
			Renderer->AddScene(test);
			Renderer->SetCurrentScene(test->GetSceneId());

			return true;
		}
	}

	// TODO
	// Load assets, PSO(on the other thread)

	return false;
}

int Application::Run()
{
	// TODO
	// 1. I need to create a render thread which manages render commands.
	// 2. Some of functions in Renderer (like belows) has to be called in the render thread.
	// I'm not sure that making a game thread and rhi thread is a great strategy in a perspective of code management.
	// So far, I've only decided to plan the render thread.

	Renderer->BeginRender();

	ViewportRenderer->BeginDrawWindow(GCommandContext.GetCommandList());

	// GCommandContext has to be managed in the render thread, and the thread is the only place we can access it.
	//ViewportRenderer->Draw(GCommandContext.GetCommandList());

	Renderer->Render(GCommandContext.GetCommandList());

	Renderer->EndRender();
	ViewportRenderer->EndDrawWindow(GCommandContext.GetCommandList());

	return 1;
}

GameTimer& Application::GetTimer() const
{
	return *InputManager::Get().GetTimer();
}