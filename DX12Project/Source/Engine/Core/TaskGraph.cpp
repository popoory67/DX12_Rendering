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

void TaskGraphSystem::Execute(ThreadType InThreadType)
{
    const auto& it = TaskGraphs.find(InThreadType);
    if (it != TaskGraphs.cend())
    {
        (it->second)->Execute();
    }
}
