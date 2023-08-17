#include "Application.h"
#include "InputManager.h"
#include "ThreadBase.h"
#include "GameTimer.h"
#include "RenderThread.h"
#include "GameThread.h"
#include "CommandList.h"
#include "Viewport.h"

void* Application::MainWindowHandle = nullptr;

Application::Application()
	: Caption(L"Application")
	, ClientWidth(800)
	, ClientHeight(600)
{
    Timer = std::make_unique<GameTimer>();
}

Application::~Application()
{
    RenderThread::StopRenderThread();
    GameThread::StopGameThread();
}

bool Application::Initialize()
{
    GameThread::StartGameThread();
    RenderThread::StartRenderThread();

	return true;
}

int Application::Run()
{
	return 1;
}

void Application::ResetTimer() const
{
	Timer->Reset();
}

void Application::TickTimer() const
{
	Timer->Tick();
}

void* Application::GetWindowHandle()
{
    return MainWindowHandle;
}