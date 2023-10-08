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
	void AddCommandList(std::unique_ptr<RHICommandList>&& InCommandList);
	void AddCommand(struct RHICommand* InCommand) const;
	void ExecuteCommands();

	void CleanUp();

private:
	void Close();

private:
	std::vector<std::unique_ptr<RHICommandList>> CommandLists;
	unsigned int CurrentCommandListHandle = 0;

	mutable std::list<std::unique_ptr<struct RHICommand>> Commands;

	mutable bool bClose = false;
};
