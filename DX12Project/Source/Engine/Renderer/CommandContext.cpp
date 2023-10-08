#include "CommandContext.h"
#include "CommandList.h"
#include "RenderInterface.h"
#include <assert.h>

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
    std::unique_ptr<RHICommand> command(InCommand);

    if (command->GetPriority() == CommandPriority::First)
    {
        Commands.push_front(std::move(command));
    }
    else
    {
        Commands.push_back(std::move(command));
    }
}

void RHICommandContext::ExecuteCommands()
{
    // TODO
    // Processing commands on a concurrency task with a priority
    bClose = false;

    while (!Commands.empty() && !bClose)
    {
        std::unique_ptr<RHICommand> command = std::move(Commands.front());
        command->Execute(*this);

        Commands.pop_front();
    }

    Close();
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