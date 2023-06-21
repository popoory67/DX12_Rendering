#pragma once
#include "RenderInterface.h"

class RHICommandList;

class Viewport
{
public:
	Viewport() = delete;
	explicit Viewport(RHIViewport*&& InViewportRHI);
	virtual ~Viewport();

	void Draw(RHICommandList& InCommandList);
	void BeginDrawWindow(RHICommandList& InCommandList);
	void EndDrawWindow(RHICommandList& InCommandList);

private:
	RHIViewport* ViewportInterface;
};