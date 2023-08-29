#pragma once
#include "RenderInterface.h"

class RHICommandContext;

class Viewport
{
public:
	Viewport() = delete;
	explicit Viewport(RHIViewport*&& InViewportRHI);
	virtual ~Viewport();

	void Draw(RHICommandContext& InContext);

private:
	RHIViewport* ViewportInterface;
};