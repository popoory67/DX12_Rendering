#pragma once
#include "Util.h"
#include <memory>
#include <unordered_map>
#include <queue>
#include <assert.h>

class TaskGraphBase : public Uncopyable
{
	friend class TaskGraphSystem;

public:
	TaskGraphBase() = default;
	virtual ~TaskGraphBase() = default;

	virtual void Execute() {}
	virtual bool IsCompleted() { return true; }

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
		TaskInternal = std::make_unique<TaskType>(std::forward<Lambda>(InLambda));
	}

	void Execute()
	{
		if (Prerequisite && !Prerequisite->IsCompleted())
		{
			Prerequisite->Execute();
			return;
		}

		if (!IsCompleted())
		{
			TaskInternal->DoTask();

			bCompleted = true;

			// TODO
			// concurrency switching
		}
	}

	FORCEINLINE bool IsCompleted() override
	{
		return bCompleted;
	}

protected:
	std::unique_ptr<TaskType> TaskInternal;

	bool bCompleted = false;
};

class TaskGraphSystem : public Uncopyable
{
public:
	virtual ~TaskGraphSystem();

	static TaskGraphSystem& Get();
	
	// test
	enum class ThreadType GetThreadState() const { return ThreadState; }
	void SetThreadState(enum class ThreadType InThreadState) { ThreadState = InThreadState; }

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
			AddTask(task, InThreadType);
		}
	}

	void AddTask(TaskGraphBase* InTaskGraph, enum class ThreadType InThreadType);

	void Execute(enum class ThreadType InThreadType);

private:
	TaskGraphSystem();

	void Destroy();

private:
	enum class ThreadType ThreadState;

	std::unordered_map<ThreadType, TaskGraphBase*> TaskGraphs;
};
