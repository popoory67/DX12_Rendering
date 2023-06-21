#pragma once

class RHIViewport;

// RHI is processed in the main thread
// for the reason that the role of this interface is to process what we have to execute in CPU.
// If you wanna know how to handle any render commands,
// you need to see the command list interface. (CommandList.h)
class RHI
{
public:
	RHI() = default;
	virtual ~RHI() = default;

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;

	virtual RHIViewport* CreateViewport(void* InHWnd, unsigned int InWidth, unsigned int InHeight) = 0;
	virtual void ResizeViewport(RHIViewport* InViewportRHI) = 0;
};

class RHIViewport
{
public:
	RHIViewport() = default;
	virtual ~RHIViewport() = default;

	RHIViewport(const RHIViewport&) = delete;
	const RHIViewport& operator=(const RHIViewport&) = delete;
};