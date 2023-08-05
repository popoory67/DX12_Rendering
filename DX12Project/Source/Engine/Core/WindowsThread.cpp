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

}

void WindowsThread::Kill()
{

}

bool WindowsThread::CreateInternal(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority)
{
    assert(InAction);

    Action = InAction;
    Type = InThreadType;

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
