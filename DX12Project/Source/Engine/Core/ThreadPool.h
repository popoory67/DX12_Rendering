#pragma once
#include "ThreadBase.h"

class ThreadPool : public Uncopyable
{
	friend class Application;

public:
	virtual ~ThreadPool();

	static ThreadPool& Get();

	void Enqueue(Task* InTask, ThreadType InType = ThreadType::Worker);

private:
	void Run();
	void StopAll();

	int GetAvailableThreadHandle() const;

	ThreadPool();

private:
	std::vector<std::unique_ptr<GenericThread>> Backgrounds;
	std::queue<Task*> Tasks;

	std::mutex Mutex;

	bool bStopAll = false;

	const int PoolSize = 2;
};