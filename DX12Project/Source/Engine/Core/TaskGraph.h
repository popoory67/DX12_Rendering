#pragma once
#include "ThreadBase.h"
#include <vector>

class Task
{
public:
	virtual bool Init() = 0;
	virtual void Run() = 0;
	virtual void Stop()
	{
		bCompleted = true;
		bReady = true;
	}

	bool IsCompleted() const { return bCompleted; }
	bool IsReady() const { return bReady; }

private:
	bool bCompleted = true;
	bool bReady = true;
};

template<class T>
class TaskGraph
{
	//std::vector<Task*> Dependencies;
};