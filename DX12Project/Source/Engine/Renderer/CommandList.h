#pragma once
#include "Util.h"

extern class RHICommandContext GCommandContext;

// This class is for render commands which will be processed in GPU.
// And it's better to be executed in a render thread, not a main thread.
// The key point of a command list is to collect and push commands to GPU.
class RHICommandList
{
public:
	RHICommandList() = default;
	virtual ~RHICommandList() = default;

	virtual void BeginDrawWindow(class RHIViewport* InViewport) = 0;
	virtual void EndDrawWindow(class RHIViewport* InViewport) = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void AddCommand(struct RHICommand*&& InCommand) const = 0;
};

class RHICommandListExecutor
{
public:
	RHICommandListExecutor() = default;
	virtual ~RHICommandListExecutor() = default;

	virtual void ExecuteCommandLists(RHICommandList* InCommandList) = 0;
	virtual void FlushCommandLists() = 0;
};

class RHICommandContext
{
public:
	void SetCommandList(RHICommandList* InCommandList);

	// This must make sure that it never return nullptr in run-time.
	RHICommandList& GetCommandList() const;

private:
	RHICommandList* CommandList = nullptr;
};

struct RHICommand : public Uncopyable
{
public:
	RHICommand() = default;
	virtual ~RHICommand() = default;

	virtual void Execute(const RHICommandList& InCmdList) = 0;
};

template<class TCommand>
struct RHICommandBase : public RHICommand
{
public:
	RHICommandBase() = default;
	virtual ~RHICommandBase() = default;

	void ExecuteAndDestruct(const RHICommandList& InCmdList)
	{
		TCommand* pCmd = static_cast<TCommand*>(this);
		if (pCmd)
		{
			pCmd->Execute(InCmdList);
			pCmd->~TCommand();
		}
	}
};