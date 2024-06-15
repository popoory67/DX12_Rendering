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
    bSuspended = Action->CheckCondition();

    Condition.notify_all();
}

void WindowsThread::Wait()
{
    std::unique_lock<std::mutex> lock(Mutex);
    Condition.wait(lock, [this]
    {
        return !IsSuspended();
    });
}

void WindowsThread::Kill()
{
    bStop = true;

    if (!Action)
    {
        std::unique_lock<std::mutex> lock(Mutex);
        Condition.notify_all();
        return;
    }

    Action->Stop();
    Resume();
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
    if (!IsTaskAllocated())
    {
        std::unique_lock<std::mutex> lock{ Mutex };
        Condition.wait(lock, [this]()
        {
            return IsTaskAllocated() || bStop;
        });
    }

    if (bStop)
    {
        Kill();
        return;
    }

    if (!Action->Init())
    {
        Kill();
        return;
    }

    while (!Action->IsStopped())
    {
        Action->Run();
        bSuspended = Action->CheckCondition();

        // Wait for the next resume after updating once
        if (bSuspended)
        {
            Wait();
        }
    }
    Kill();
}
