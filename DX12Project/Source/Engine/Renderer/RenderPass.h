#pragma once
#include "TaskGraph.h"

enum class RenderPassType : unsigned
{
	None = 0,
	Base = 1,
	Transparent = 2,
};

// Batching is to process jobs at once, which reduces overhead.
struct RenderBatch
{
public:
	RenderBatch() = default;
	virtual ~RenderBatch() = default;

	RenderBatch(RenderBatch&& InBatch)
	{
		PipelineStateId = InBatch.PipelineStateId;
	}

protected:
	int PipelineStateId = -1;
};

// Render Pass is a stage to render in the rendering pipeline in a specific order and sometimes dependency.
// The meaning of "Pass" is more of the feature that has a particular effect,
// for example, base color, lighting, shadows, reflection, and so on.
// It is sometimes compared with the understanding of an image layer, but not the same thing.
// Because the pass contains the meaning of process, such as the process to calculate the base color with textures and modeling assets and draw a 2D texture to frame buffer on VRAM.
class RenderPass
{
public:
	RenderPass();
	virtual ~RenderPass() = default;
	RenderPass(RenderPass&&) = default;
	RenderPass& operator=(RenderPass&&) = default;

	virtual void DoTask() {}
	constexpr int GetPriority() { return Priority; }

protected:
	int Priority;
	RenderPassType Type;
};

class RenderGraph : public TaskGraph<RenderPass>
{
public:
	RenderGraph() = default;
	virtual ~RenderGraph() = default;

	static RenderGraph& Get();

	void AddTask(std::unique_ptr<RenderPass> InRenderPass);

	// TODO
	// Manage and clear tasks
};