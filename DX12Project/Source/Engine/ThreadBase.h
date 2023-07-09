#pragma once
#include <thread>
#include "Util.h"

enum class ThreadPriority : unsigned
{
	Lowest = 0,
	Low,
	Normal,
	High,
	Immediate,
};

class WorkingUnit
{
public:
	virtual bool Init() { return true; }
	virtual void Run() = 0;
	virtual void Stop() {}
};

class ThreadBase : public Noncopyable
{
public:
	static ThreadBase* Create(WorkingUnit* InAction, ThreadPriority InPriority);

	virtual void Suspend() = 0;
	virtual void Kill() = 0;

protected:
	virtual bool CreateInternal(WorkingUnit* InAction, ThreadPriority InPriority) = 0;

protected:
	WorkingUnit* Action;
	ThreadPriority Priority;
};

// WindowsThread will be a base class of thread
// because this project didn't be thought of other platforms.
class WindowsThread : public ThreadBase
{
public:
	static ThreadBase* CreateThread();

	void Suspend() override {}
	void Kill() override {}

	bool CreateInternal(WorkingUnit* InAction, ThreadPriority InPriority);

protected:
	virtual void ThreadProc() {}

protected:
	std::thread Thread;
};