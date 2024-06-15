#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    Backgrounds.reserve(PoolSize);

    for (int i = 0; i < PoolSize; ++i)
    {
        GenericThread* rawThread = GenericThread::Create(nullptr, ThreadType::Worker);

        std::unique_ptr<GenericThread> workerThread(rawThread);
        Backgrounds.emplace_back(std::move(workerThread));
    }
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

void ThreadPool::Enqueue(Task* InTask, ThreadType InType)
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

        Task* task = Tasks.front();
        Tasks.pop();

        int threadHandle = GetAvailableThreadHandle();
        {
            Backgrounds[threadHandle]->SetTask(task);
            Backgrounds[threadHandle]->Resume();
        }
    }
}

void ThreadPool::StopAll()
{
    bStopAll = true;

    for (const auto& workerThread : Backgrounds)
    {
        workerThread->Kill();
    }

    Backgrounds.clear();
}

int ThreadPool::GetAvailableThreadHandle() const
{
    for (int threadHandle = 0; threadHandle < PoolSize; ++threadHandle)
    {
        if (!Backgrounds[threadHandle]->IsTaskAllocated())
        {
            return threadHandle;
        }
    }
    return -1;
}
