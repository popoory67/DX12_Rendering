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
    //for (const auto& it : TaskGraphs)
    //{
    //    it->second;
    //}
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
        TaskGraphs[InThreadType] = InTaskGraph;
    }

    InTaskGraph->Subsequent = nullptr;
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
}
