#pragma once
#include "Util.h"
#include "Commands.h"
#include <memory>
#include <queue>
#include <vector>
#include <mutex>

extern class RHICommandContext GCommandContext;

static class RHICommandContext& GetCommandContext()
{
	return GCommandContext;
}

class RHICommandList;

class RHICommandContext : public Uncopyable
{
	template<typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type>>
	class PriorityQueue : public std::priority_queue<T, Container, Compare>
	{
	public:
		auto pop_move()
		{
			auto& container = this->c;
			std::pop_heap(container.begin(), container.end(), this->comp);

			auto moved = std::move(container.back());
			container.pop_back();

			return moved;
		}
	};

	struct Compare
	{
		bool operator()(const auto& a, const auto& b)
		{
			return a->GetPriority() > b->GetPriority();
		}
	};

	using PriorityQueue_Commands = PriorityQueue<std::unique_ptr<struct RHICommand>, std::vector<std::unique_ptr<struct RHICommand>>, Compare>;

public:
	static const int GetCommandListCount();

	RHICommandList& GetCommandList() const;
	void AddCommandList(std::unique_ptr<RHICommandList>&& InCommandList);
	void AddCommand(struct RHICommand* InCommand) const;
	void ExecuteCommands();

	void CleanUp();

private:
	void Close();

	void ExecuteCommandsInternal(PriorityQueue_Commands& InExecutable, PriorityQueue_Commands& InBackup);

private:
	std::vector<std::unique_ptr<RHICommandList>> CommandLists;
	unsigned int CurrentCommandListHandle = 0;
	static const int CommandListCount;

	mutable PriorityQueue_Commands Commands;
	mutable PriorityQueue_Commands BackupCommands;

	mutable bool bClose = false;

	mutable std::mutex Mutex;
};
