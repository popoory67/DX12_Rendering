#pragma once

#include "FrameResource.h"

// resource params
class D3DViewportResource : public ResourceBase
{
public:
	FLOAT TopLeftX;
	FLOAT TopLeftY;
	FLOAT Width;
	FLOAT Height;
	FLOAT MinDepth;
	FLOAT MaxDepth;
};

class CommandListBase
{
public:
	CommandListBase()
	{

	}

	virtual ~CommandListBase() {}

	//virtual void CreateAndSetViewports(D3DViewportResource& InResource) {}
};

class CommandListExecutor
{
public:
	virtual void Execute(CommandListBase& InCommandList) = 0;
	virtual void FlushCommands() = 0;
};

template<class TCommand>
struct CommandBase
{
	CommandBase() {}
	virtual ~CommandBase() {}

	virtual void Execute(CommandListBase& InCmdList)
	{

	}

	void ExecuteAndDestruct(CommandListBase& InCmdList)
	{
		TCommand* pCmd = static_cast<TCommand>(this);
		if (pCmd)
		{
			pCmd->Execute(InCmdList);
			pCmd->~TCommand();
		}
	}
};

static CommandListExecutor* GCommandListExecutor;