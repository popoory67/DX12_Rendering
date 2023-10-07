#pragma once
#include "Util.h"
#include <memory>
#include <queue>
#include <vector>
#include <list>

extern class RHICommandContext GCommandContext;

class RHICommandList;

class RHICommandContext : public Uncopyable
{
public:
	~RHICommandContext() = default;

	// This must make sure that it never return nullptr in run-time.
	RHICommandList& GetCommandList() const;
	void AddCommandList(RHICommandList*&& InCommandList);
	void AddCommand(struct RHICommand* InCommand) const;
	void ExecuteCommands() const;
	void Close() const;

	void CleanUp();

private:
	std::vector<RHICommandList*> CommandLists;
	mutable unsigned int CurrentCommandListHandle = 0;

	mutable std::list<std::unique_ptr<struct RHICommand>> Commands;

	mutable bool bClose = false;
};
