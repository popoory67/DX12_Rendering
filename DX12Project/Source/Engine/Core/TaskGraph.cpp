#include "TaskGraph.h"
#include "ThreadBase.h"

TaskGraphSystem::TaskGraphSystem()
{
}

TaskGraphSystem& TaskGraphSystem::Get()
{
    static TaskGraphSystem TaskInterface;
    return TaskInterface;
}

TaskGraphSystem::~TaskGraphSystem()
{
    Destroy();
}

void TaskGraphSystem::Destroy()
{
    for (const auto& it : TaskGraphs)
    {
        if (TaskGraphBase* graph = it.second)
        {
            while (graph)
            {
                TaskGraphBase* temporary = graph;
                graph = graph->Prerequisite;

                //temporary->Execute();

                SafeDelete(temporary);
            }
        }
    }
}

void TaskGraphSystem::AddTask(TaskGraphBase* InTaskGraph, ThreadType InThreadType)
{
    if (TaskGraphs[InThreadType])
    {
        // TaskGraphs[InThreadType] -> New Task
        InTaskGraph->Prerequisite = TaskGraphs[InThreadType];
        TaskGraphs[InThreadType]->Subsequent = InTaskGraph;
    }
    else
    {
        // New task
        assert(InThreadType < ThreadType::Foreground);
        TaskGraphs[InThreadType] = InTaskGraph;
    }

    InTaskGraph->Subsequent = nullptr;
    TaskGraphs[InThreadType] = InTaskGraph;
}

void TaskGraphSystem::Execute(ThreadType InThreadType)
{
    const auto& it = TaskGraphs.find(InThreadType);
    if (it != TaskGraphs.cend())
    {
        if (TaskGraphBase* graph = (it->second))
        {
            graph->Execute();

            TaskGraphBase* subsequent = graph->Subsequent;
            it->second = subsequent;

            while (graph && graph->IsCompleted())
            {
                // TODO
                // Graph memory pool
                TaskGraphBase* temporary = graph;
                graph = graph->Prerequisite;

                SafeDelete(temporary);
            }

            if (subsequent)
            {
                subsequent->Prerequisite = graph;
            }
        }
    }

    int state = (static_cast<int>(ThreadState) + 1) % static_cast<int>(ThreadType::Foreground);
    ThreadState = static_cast<ThreadType>(state);
}
