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

class Task : public Uncopyable
{
public:
	virtual ~Task() = default;

	virtual bool Init() = 0;
	virtual void Run() = 0;
	virtual void Stop() { bStop = true; }

	virtual bool CheckCondition() { return !IsStopped(); }

	bool IsStopped() { return bStop; }

private:
	bool bStop = false;
};

class GenericThread : public Uncopyable
{
public:
	virtual ~GenericThread();

	static GenericThread* Create(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority = ThreadPriority::Normal);

	// Temporarily stop this thread not to use system resources.
	// This function is usually called for waiting a specific resource.
	virtual void Suspend() = 0;

	// Run this thread back on.
	virtual void Resume() = 0;

	// Force stop this thread.
	virtual void Kill() = 0;

	bool IsTaskAllocated() const;
	void SetTask(Task* InTask);

protected:
	virtual bool CreateInternal(Task* InAction, ThreadType InThreadType, ThreadPriority InPriority) = 0;

protected:
	Task* Action = nullptr;

	ThreadPriority Priority;
	ThreadType Type;
	std::atomic<bool> bStop;
};
