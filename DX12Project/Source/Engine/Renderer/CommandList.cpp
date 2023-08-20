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
