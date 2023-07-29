#include "Application.h"
#include "InputManager.h"
#include "CommandList.h"
#include "Viewport.h"
#include "RenderThread.h"

void* Application::MainWindowHandle = nullptr;

Application::Application()
	: Caption(L"Application")
	, ClientWidth(800)
	, ClientHeight(600)
{

}

Application::~Application()
{
	RenderThread::StopRenderThread();
}

bool Application::Initialize()
{
    RenderThread::StartRenderThread();

	return true;
}

int Application::Run()
{
	return 1;
}

GameTimer& Application::GetTimer() const
{
	return *InputManager::Get().GetTimer();
}

void* Application::GetWindowHandle()
{
    return MainWindowHandle;
}