#pragma once
#include "Commands.h"
#include <memory>

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
	virtual void ResizeViewport(class RHIViewport* InViewport) = 0;
	virtual void SetRenderTargets(class RHIRenderTargetInfo* InRenderTargets, unsigned int InNumRenderTarget, class RHIResource* InDepthStencil) = 0;
	virtual void AddResource(class RHIResource*&& InResource) = 0;
	virtual void SetStreamResource(class RHIResource* InVertexBuffer, const UINT InIndicesSize) = 0;
	virtual void DrawPrimitive(unsigned int InNumVertices, unsigned int InNumInstances, unsigned int InStartIndex, unsigned int InStartInstance) = 0;
	virtual void DrawIndexedInstanced(RHIResource* InVertexBuffer, unsigned int InNumIndices, unsigned int InNumInstances, unsigned int InStartIndex, int InStartVertex, unsigned int InStartInstance) = 0;
};

class RHICommandListExecutor
{
public:
	RHICommandListExecutor() = default;
	virtual ~RHICommandListExecutor() = default;

	virtual void ExecuteCommandLists(RHICommandList* InCommandList) = 0;
	virtual void FlushCommandLists() = 0;
};