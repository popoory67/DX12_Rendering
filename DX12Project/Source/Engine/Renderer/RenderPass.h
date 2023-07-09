#pragma once

enum class RenderPassType : unsigned
{
	None = 0,
	Base = 1,
	Transparent = 2,
};

// Batching is to process jobs at once, which reduces overhead.
struct RenderBatch
{
protected:
	int PipelineStateId = -1;
};

// Render Pass is a stage to render in the rendering pipeline in a specific order.
// The meaning of "Pass" is more of the feature that has a particular effect,
// for example, base color, lighting, shadows, reflection, and so on.
// It is sometimes compared with the understanding of an image layer, but not the same thing.
class RenderPass
{
public:
	RenderPass();
	virtual ~RenderPass() = default;

protected:
	virtual void Process(class RHICommandList& InCommandList) = 0;

protected:
	int Priority;
	RenderPassType Type;
};

class RenderGraph
{

};