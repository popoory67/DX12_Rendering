#pragma once
#include "ThreadBase.h"
#include <thread>

// WindowsThread will be a base class of thread
// because this project didn't be considered of other platforms.
class WindowsThread final : public GenericThread
{
public:
	WindowsThread() = default;
	virtual ~WindowsThread();

	static GenericThread* CreateThread();

	void Suspend() override;
	void Resume() override;
	void Kill() override;

	bool CreateInternal(class Task* InAction, ThreadType InThreadType, ThreadPriority InPriority) override;

protected:
	void ThreadProc();

	void Wait();
	bool IsSuspended() const;

protected:
	std::thread Thread;

	std::condition_variable Condition;
	std::mutex Mutex;

	bool bSuspended = false;
};