#include "Application.h"
#include "InputManager.h"
#include "ThreadBase.h"
#include "GameTimer.h"
#include "RenderThread.h"
#include "GameThread.h"
#include "CommandList.h"
#include "Viewport.h"
#include <condition_variable>
#include <mutex>

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
    std::shared_ptr<std::mutex> mutex = std::make_shared<std::mutex>();
    std::shared_ptr<std::condition_variable> condition = std::make_shared<std::condition_variable>();

    GameThread::StartGameThread(mutex, condition);
    RenderThread::StartRenderThread(mutex, condition);

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

float Application::GetWidth()
{
    return ClientWidth;
}

float Application::GetHeight()
{
    return ClientHeight;
}
