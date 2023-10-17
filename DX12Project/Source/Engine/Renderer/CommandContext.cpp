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
    std::unique_ptr<RHICommand> command(InCommand);

    Commands.push(std::move(command));
}

void RHICommandContext::ExecuteCommands()
{
    // TODO
    // Processing commands on a concurrency task with a priority
    bClose = false;

    while (!Commands.empty() && !bClose)
    {
        std::unique_ptr<RHICommand> command = Commands.pop_move();// std::move(Commands.top());
        command->Execute(*this);
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