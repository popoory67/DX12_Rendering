#include "Application.h"
#include "InputManager.h"
#include "CommandList.h"
#include "Viewport.h"
#include "RenderThread.h"
#include "TaskGraph.h"
#include "ThreadBase.h"
#include "GameTimer.h"

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
}

bool Application::Initialize()
{
    RenderThread::StartRenderThread();

	return true;
}

int Application::Run()
{
	TaskGraphSystem::Get().Execute(ThreadType::Render);

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