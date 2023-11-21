#pragma once
#include "Util.h"
#include <memory>
#include <unordered_map>
#include <queue>
#include <assert.h>

class TaskNode
{
public:
	TaskNode() = default;
	virtual ~TaskNode() = default;

	TaskNode(const TaskNode&) = default;
	TaskNode(TaskNode&&) = default;
	TaskNode& operator=(TaskNode&&) = default;

	std::vector<unsigned int>& GetDependencies()
	{
		return Dependencies;
	}

protected:
	std::vector<unsigned int> Dependencies;
};

class TaskGraphBase : public Uncopyable
{
	friend class TaskGraphSystem;

public:
	TaskGraphBase() = default;
	virtual ~TaskGraphBase() = default;

	virtual void Execute() {}
};

template<class TaskType = TaskNode>
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
		std::unique_ptr<TaskType> task = std::make_unique<TaskType>(std::forward<Lambda>(InLambda));

		AddTask(std::move(task));
	}

	void AddTask(std::unique_ptr<TaskType> InTaskNode)
	{
		SetHighest(InTaskNode->GetPriority());

		NextTaskGraph.emplace(InTaskNode->GetPriority(), std::move(InTaskNode));
	}

	void Execute() override
	{
		if (CurrentTaskGraph.empty())
		{
			SwapBuffers();
		}

		while (!HighestTasks.empty())
		{
			unsigned int taskId = HighestTasks.front();
			HighestTasks.pop();

			if (CurrentTaskGraph.find(taskId) != CurrentTaskGraph.cend())
			{
				// TODO
				// multithreading
				std::unique_ptr<TaskType> task = std::move(CurrentTaskGraph[taskId]);
				task->DoTask();

				for (unsigned int id : task->GetDependencies())
				{
					HighestTasks.push(id);
				}
				CurrentTaskGraph.erase(taskId);
			}
		}
	}

private:
	void SwapBuffers()
	{
		CurrentTaskGraph = std::move(NextTaskGraph);
	}

	void SetHighest(unsigned int InPriority)
	{
		if (HighestTasks.empty())
		{
			HighestTasks.push(InPriority);
		}
	}

private:
	std::unordered_map<unsigned int, std::unique_ptr<TaskType>> CurrentTaskGraph;
	std::unordered_map<unsigned int, std::unique_ptr<TaskType>> NextTaskGraph;

	std::queue<unsigned int> HighestTasks;
};