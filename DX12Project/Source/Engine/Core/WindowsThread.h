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
	void Kill() override;

	bool CreateInternal(class Task* InAction, ThreadPriority InPriority) override;

protected:
	void ThreadProc();

protected:
	std::thread Thread;
};