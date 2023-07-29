#pragma once
#include <thread>
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

class WindowsThread;

template<class T>
class ThreadBase : public Uncopyable
{
public:
	static T* Create(Task* InAction, ThreadPriority InPriority = ThreadPriority::Normal);

	virtual void Suspend() = 0;
	virtual void Kill() = 0;

protected:
	virtual bool CreateInternal(Task* InAction, ThreadPriority InPriority) = 0;

protected:
	Task* Action;
	ThreadPriority Priority;
};

template<class T>
T* ThreadBase<T>::Create(Task* InAction, ThreadPriority InPriority)
{
	// It would be much better to be abstracted depending on target platforms.
	// More specifically if I would say my idea,
	// WindowsThread need to be created by another class that selects and manages platforms.
	// However, this project is for DirectX 12, so I didn't think other platforms so far.
	T* newThread = WindowsThread::CreateThread();

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

// WindowsThread will be a base class of thread
// because this project didn't be considered of other platforms.
class WindowsThread final : public ThreadBase<WindowsThread>
{
public:
	WindowsThread() = default;
	virtual ~WindowsThread();

	static WindowsThread* CreateThread();

	void Suspend() override;
	void Kill() override;

	bool CreateInternal(Task* InAction, ThreadPriority InPriority) override;

protected:
	void ThreadProc();

protected:
	std::thread Thread;
};

// thread pool