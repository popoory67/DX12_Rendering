#include "CommandContext.h"
#include "CommandList.h"
#include "RenderInterface.h"
#include <assert.h>

RHICommandContext GCommandContext;

void RHICommandContext::SetCommandList(RHICommandList* InCommandList)
{
    CommandList = InCommandList;
}


RHICommandList& RHICommandContext::GetCommandList() const
{
    assert(CommandList);
    return *CommandList;
}

void RHICommandContext::AddCommand(struct RHICommand*&& InCommand) const
{
    std::unique_ptr<RHICommand> command(std::move(InCommand));
    if (command->GetPriority() == CommandPriority::First)
    {
        Commands.push_front(std::move(command));
    }
    else
    {
        Commands.push_back(std::move(command));
    }
}

void RHICommandContext::ExecuteCommands() const
{
    // TODO
    // Processing commands on a concurrency task with a priority
    bClose = false;

    while (!Commands.empty() && !bClose)
    {
        std::unique_ptr<RHICommand> command= std::move(Commands.front());
        command->Execute(*this);

        Commands.pop_front();
    }
}