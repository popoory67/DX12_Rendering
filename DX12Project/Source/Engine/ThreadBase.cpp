#include "stdafx.h"
#include "ThreadBase.h"
#include <thread>

ThreadBase* ThreadBase::Create(WorkingUnit* InAction, ThreadPriority InPriority)
{
    // It would be much better to be abstracted depending on target platforms.
    // More specifically if I would say my idea,
    // WindowsThread need to be created by another class that selects and manages platforms.
    // However, this project is for DirectX 12, so I didn't think other platforms so far.
    ThreadBase* newThread = WindowsThread::CreateThread();

    if (newThread)
    {
        bool bValid = newThread->CreateInternal(InAction, InPriority);
        if (!bValid)
        {
            SafeDelete(newThread);
        }
    }
    return newThread;
}

ThreadBase* WindowsThread::CreateThread()
{
    return new WindowsThread();
}

bool WindowsThread::CreateInternal(WorkingUnit* InAction, ThreadPriority InPriority)
{
    Thread = std::thread{ &WindowsThread::ThreadProc, this };

    return true;
}
