#pragma once
#include "Util.h"

enum class ThreadType : int
{
	Main = 0,
	Render,
	Worker,
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

// thread pool