#pragma once
#include "Util.h"
#include <condition_variable>
#include <mutex>
#include <queue>

enum class ThreadType : int
{
	Main = 0,
	Render,
	Foreground,

	Worker, // background
};

enum class ThreadPriority : unsigned
{
	Lowest = 0,
	Low,
	Normal,
	High,
	Immediate,
};

class Task
{
public:
	virtual ~Task() = default;

	virtual bool Init() = 0;
	virtual void Run() = 0;
	virtual void Stop() {}
};

class GenericThread : public Uncopyable
{
public:
	virtual ~GenericThread();

	static GenericThread* Create(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority = ThreadPriority::Normal);

	virtual void Suspend() = 0;
	virtual void Kill() = 0;

protected:
	virtual bool CreateInternal(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority) = 0;

protected:
	Task* Action = nullptr;

	ThreadPriority Priority;
	ThreadType Type;
};

class ThreadPool : public Uncopyable
{
public:
	ThreadPool();
	virtual ~ThreadPool();

	static ThreadPool& Get();

	void Enqueue(Task*&& InTask, ThreadType InType = ThreadType::Worker);

private:
	void Run();
	void StopAll();

private:
	std::vector<std::thread> Backgrounds; // Worker threads
	std::queue<Task*> Tasks;

	std::condition_variable Condition;
	std::mutex Mutex;

	bool bStopAll = false;

	const int PoolSize = 2;
};