#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

// TODO
// It has to be modified to a module.
namespace RenderThread
{
	void StartRenderThread(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition);
	void StopRenderThread();
};

// Command is a simple and running once.
struct RenderCommand
{
	using Lambda = std::function<void(const class RHICommandContext&)>;

	RenderCommand() = delete;
	RenderCommand(Lambda&& InLambda)
		: Func(std::forward<Lambda>(InLambda))
	{

	}
	virtual ~RenderCommand() = default;

	void DoTask();

private:
	Lambda Func;
};