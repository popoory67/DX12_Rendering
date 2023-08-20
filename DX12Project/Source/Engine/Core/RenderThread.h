#pragma once
#include <functional>

// TODO
// It has to be modified to a module.
namespace RenderThread
{
	void StartRenderThread();
	void StopRenderThread();
};

// Command is a simple and running once.
struct RenderCommand
{
	using Lambda = std::function<void(const class RHICommandList&)>;

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