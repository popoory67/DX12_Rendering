#include "CommandContext.h"
#include "CommandList.h"
#include "RenderInterface.h"
#include <assert.h>

const int RHICommandContext::CommandListCount = 1; // TODO : it would be managed more than 1

const int RHICommandContext::GetCommandListCount()
{
    return CommandListCount;
}

void RHICommandContext::AddCommandList(std::unique_ptr<RHICommandList>&& InCommandList)
{
    CommandLists.emplace_back(std::move(InCommandList));
}

RHICommandList& RHICommandContext::GetCommandList() const
{
    return *CommandLists[CurrentCommandListHandle];
}

void RHICommandContext::AddCommand(RHICommand* InCommand) const
{
    std::lock_guard<std::mutex> lock(Mutex);
    {
        std::unique_ptr<RHICommand> command(InCommand);
        if (command->GetPriority() == PipelinePrioirty::Draw)
        {
            bClose = false;
        }
        Commands.push(std::move(command));
    }
}

void RHICommandContext::ExecuteCommands()
{
    std::lock_guard<std::mutex> lock(Mutex);
    {
        if (bClose)
        {
            ExecuteCommandsInternal(BackupCommands, Commands);
            BackupCommands = std::move(Commands);
        }
        else
        {
            ExecuteCommandsInternal(Commands, BackupCommands);
            Close();
        }
    }
}

void RHICommandContext::CleanUp()
{
    CommandLists.clear();
}

void RHICommandContext::Close()
{
    bClose = true;

    CurrentCommandListHandle = (CurrentCommandListHandle + 1) % CommandLists.size();
}

void RHICommandContext::ExecuteCommandsInternal(PriorityQueue_Commands& InExecutable, PriorityQueue_Commands& InBackup)
{
    InBackup = PriorityQueue_Commands{};

    while (!InExecutable.empty())
    {
        std::unique_ptr<RHICommand> command = InExecutable.pop_move();
        command->Execute(*this);

        InBackup.push(std::move(command));
    }
}
