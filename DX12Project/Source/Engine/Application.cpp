#include "Application.h"
#include "InputManager.h"
#include "ThreadPool.h"
#include "GameTimer.h"
#include "RenderThread.h"
#include "GameThread.h"
#include "CommandList.h"
#include "Viewport.h"
#include "PipelineState.h"

void* Application::MainWindowHandle = nullptr;
float Application::ClientWidth = 800.0f;
float Application::ClientHeight = 600.0f;

Application::Application()
	: Caption(L"Application")
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

    // Worker thread
    {
        PSOLibrary* psoCache = new PSOLibrary();
        ThreadPool::Get().Enqueue(psoCache, ThreadType::Worker);
    }

	return true;
}

int Application::Run()
{
    ThreadPool::Get().Run();
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

float Application::GetWidth()
{
    return ClientWidth;
}

float Application::GetHeight()
{
    return ClientHeight;
}
