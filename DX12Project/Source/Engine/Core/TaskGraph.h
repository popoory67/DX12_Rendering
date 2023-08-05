#pragma once
#include "Util.h"
#include <unordered_map>
#include <assert.h>

class Task
{
public:
	virtual ~Task() = default;

	virtual bool Init() = 0;
	virtual void Run() = 0;
	virtual void Stop() {}
};

class TaskGraphBase : public Uncopyable
{
	friend class TaskGraphSystem;

public:
	TaskGraphBase() = default;
	virtual ~TaskGraphBase() = default;

	virtual void Execute() = 0;
	virtual bool IsCompleted() = 0;

protected:
	TaskGraphBase* Prerequisite = nullptr; // prev
	TaskGraphBase* Subsequent = nullptr; // next
};

template<class TaskType>
class TaskGraph : public TaskGraphBase
{
public:
	TaskGraph() = default;
	virtual ~TaskGraph() = default;

	template<class Lambda>
	static TaskGraph* CreateGraph(Lambda&& InLambda)
	{
		TaskGraph* taskGraph = new TaskGraph();
		taskGraph->AddTask(std::forward<Lambda>(InLambda));

		return taskGraph;
	}

	template<class Lambda>
	void AddTask(Lambda&& InLambda)
	{
		TaskInternal = new TaskType(std::forward<Lambda>(InLambda));
	}

	void Execute()
	{
		if (Prerequisite && Prerequisite->IsCompleted())
		{
			Prerequisite->Execute();
		}

		assert(TaskInternal);
		TaskInternal->DoTask();

		bCompleted = true;
	}

	FORCEINLINE bool IsCompleted() override
	{
		return bCompleted;
	}

private:
	TaskType* TaskInternal = nullptr;

	bool bCompleted = false;
};

class TaskGraphSystem : public Uncopyable
{
public:
	virtual ~TaskGraphSystem();

	static TaskGraphSystem& Get();

	template<class TaskType, class Lambda>
	void AddTask(Lambda&& InLambda, enum class ThreadType InThreadType)
	{
		const auto& it = TaskGraphs.find(InThreadType);
		if (it == TaskGraphs.cend())
		{
			TaskGraphs[InThreadType] = TaskGraph<TaskType>::CreateGraph(std::forward<Lambda>(InLambda));
		}
		else
		{
			TaskGraphBase* task = TaskGraph<TaskType>::CreateGraph(std::forward<Lambda>(InLambda));
			task->Prerequisite = TaskGraphs[InThreadType];
			TaskGraphs[InThreadType]->Subsequent = task;
			TaskGraphs[InThreadType] = task;
		}
	}

	void Execute(enum class ThreadType InThreadType);

private:
	TaskGraphSystem();

	void Destroy();

private:
	std::unordered_map<enum class ThreadType, TaskGraphBase*> TaskGraphs;
};
