#pragma once
#include "Util.h"
#include "TaskGraph.h"

enum class ThreadPriority : unsigned
{
	Lowest = 0,
	Low,
	Normal,
	High,
	Immediate,
};

class GenericThread : public Uncopyable
{
public:
	virtual ~GenericThread() = default;

	static GenericThread* Create(Task* InAction, ThreadPriority InPriority = ThreadPriority::Normal);

	virtual void Suspend() = 0;
	virtual void Kill() = 0;

protected:
	virtual bool CreateInternal(Task* InAction, ThreadPriority InPriority) = 0;

protected:
	Task* Action;
	ThreadPriority Priority;
};

// thread pool