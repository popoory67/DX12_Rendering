#include "ThreadBase.h"
#include "Global.h"

// It would be much better to be abstracted depending on target platforms.
// More specifically if I would say my idea,
// WindowsThread need to be created by another class that selects and manages platforms.
// However, this project is for DirectX 12, so I didn't think other platforms so far.
#include "WindowsThread.h"
using ProcessPlatform = WindowsThread;

GenericThread::~GenericThread()
{
    SafeDelete(Action);
}

GenericThread* GenericThread::Create(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority)
{
    GenericThread* newThread = ProcessPlatform::CreateThread();

    if (newThread)
    {
        bool bValid = newThread->CreateInternal(InAction, InThreadType, InPriority);
        if (!bValid)
        {
            SafeDelete(newThread);
        }
    }
    return newThread;
}

ThreadPool::ThreadPool()
{
    Backgrounds.reserve(PoolSize);
}

ThreadPool::~ThreadPool()
{
    StopAll();
}

ThreadPool& ThreadPool::Get()
{
    static ThreadPool threadPool;
    return threadPool;
}

void ThreadPool::Enqueue(Task*&& InTask, ThreadType InType)
{
    if (bStopAll)
    {
        throw std::runtime_error("ThreadPool is stopped.");
    }

    if (InType > ThreadType::Foreground)
    {
        Tasks.push(std::move(InTask));
    }
}

void ThreadPool::Run()
{
    while (!bStopAll && !Tasks.empty())
    {
        std::unique_lock<std::mutex> lock(Mutex);
        Condition.wait(lock, []()
        {
            return true;
        });

        // Task run

        Condition.notify_all();
    }
}

void ThreadPool::StopAll()
{
    bStopAll = true;

    Condition.notify_all();

    for (auto& t : Backgrounds)
    {
        t.join();
    }
}
