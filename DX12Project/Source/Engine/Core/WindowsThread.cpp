#include "WindowsThread.h"
#include <assert.h>

WindowsThread::~WindowsThread()
{
    if (Thread.joinable())
    {
        Thread.join();
    }
}

GenericThread* WindowsThread::CreateThread()
{
    return new WindowsThread();
}

void WindowsThread::Suspend()
{
    std::unique_lock<std::mutex> lock(Mutex);
    bSuspended = true;
}

void WindowsThread::Resume()
{
    if (!IsSuspended())
    {
        return;
    }

    std::unique_lock<std::mutex> lock(Mutex);
    bSuspended = false;

    Condition.notify_all();
}

void WindowsThread::Wait()
{
    std::unique_lock<std::mutex> lock(Mutex);

    Condition.wait(lock, [bSuspended = this->bSuspended]
    {
        return !bSuspended;
    });
}

void WindowsThread::Kill()
{
    Action->Stop();
}

bool WindowsThread::IsSuspended() const
{
    return bSuspended;
}

bool WindowsThread::CreateInternal(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority)
{
    Action = InAction;
    Type = InThreadType;

    Thread = std::thread{ &WindowsThread::ThreadProc, this };

    if (!Action)
    {
        Suspend();
    }

    return true;
}

void WindowsThread::ThreadProc()
{
    Wait();

    if (Action->Init() == true)
    {
        Action->Run();
    }
}
