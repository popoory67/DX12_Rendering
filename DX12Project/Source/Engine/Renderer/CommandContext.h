#pragma once
#include "Util.h"
#include <memory>
#include <queue>
#include <vector>
#include <list>

extern class RHICommandContext GCommandContext;

class RHICommandList;

// test
class RHIResourceManager : public Uncopyable
{
public:
	RHIResourceManager() = default;
	virtual ~RHIResourceManager() = default;

	void AddResource(std::shared_ptr<class RHIResource>&& InResource)
	{
		GpuResources.emplace_back(std::move(InResource));
	}

private:
	std::vector<std::shared_ptr<class RHIResource>> GpuResources;
};

class RHICommandContext
{
public:
	// This must make sure that it never return nullptr in run-time.
	RHICommandList& GetCommandList() const;
	void SetCommandList(RHICommandList* InCommandList);

	void AddCommand(struct RHICommand*&& InCommand) const;
	void ExecuteCommands() const;

	// test
	void AddResource(std::shared_ptr<class RHIResource>&& InResource) const
	{
		ResourceManager.AddResource(std::move(InResource));
	}

	// test
	void Close() const { bClose = true; }

private:
	RHICommandList* CommandList = nullptr;

	mutable std::list<std::unique_ptr<struct RHICommand>> Commands;

	mutable RHIResourceManager ResourceManager;

	mutable bool bClose = false;
};
