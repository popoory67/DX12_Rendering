#include "WindowsThread.h"
#include <assert.h>

WindowsThread::~WindowsThread()
{
    if (Thread.joinable())
    {
        Thread.join();
        SafeDelete(Action);
    }
}

GenericThread* WindowsThread::CreateThread()
{
    return new WindowsThread();
}

void WindowsThread::Suspend()
{

}

void WindowsThread::Kill()
{

}

bool WindowsThread::CreateInternal(Task* InAction, ThreadPriority InPriority)
{
    assert(InAction);
    Action = InAction;

    Thread = std::thread{ &WindowsThread::ThreadProc, this };

    return true;
}

void WindowsThread::ThreadProc()
{
    if (Action->Init() == true)
    {
        Action->Run();
    }
}
