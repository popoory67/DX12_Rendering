#pragma once
#include "CommandList.h"

class ObjectCommandList : public RHICommandList
{
	
};

class ObjectCommand : public RHICommandBase<ObjectCommand>
{
public:
	void Execute(RHICommandList& InCmdList)
	{

	}
};