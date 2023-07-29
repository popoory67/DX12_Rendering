#include "ThreadBase.h"
#include <thread>
#include <assert.h>

WindowsThread::~WindowsThread()
{
    //Thread.join();
}

WindowsThread* WindowsThread::CreateThread()
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
