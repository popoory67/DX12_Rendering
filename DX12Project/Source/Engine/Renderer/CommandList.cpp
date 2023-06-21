#include "stdafx.h"
#include "CommandList.h"

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
