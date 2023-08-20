#pragma once
#include "Util.h"
#include <memory>

class RHIViewport;
class RHIResource;

class RHI
{
public:
	RHI() = default;
	virtual ~RHI() = default;

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
	virtual void ResetCommandList() = 0;

	virtual RHIViewport* CreateViewport(void* InHWnd, unsigned int InWidth, unsigned int InHeight) = 0;
	virtual void ResizeViewport(RHIViewport* InViewportRHI) = 0;
	virtual std::shared_ptr<RHIResource> CreateVertexBuffer(unsigned int InSize, unsigned int InStride) = 0;
	virtual void* LockBuffer(std::shared_ptr<RHIResource> InVertexBuffer) = 0;
	virtual void UnlockBuffer(std::shared_ptr<RHIResource> InVertexBuffer) = 0;
};

class RHIViewport : public Uncopyable
{
public:
	RHIViewport() = default;
	virtual ~RHIViewport() = default;
};

class RHIResource
{
public:
	RHIResource() = default;
	virtual ~RHIResource() = default;

	virtual void* Lock() = 0;
	virtual void Unlock() = 0;

	FORCEINLINE unsigned int GetSize() const noexcept { return Size; }
	FORCEINLINE unsigned int GetStride() const noexcept { return Stride; }

protected:
	unsigned int Size = 0;
	unsigned int Stride = 0;
};

extern RHI* GRHI;

namespace RenderInterface
{
	RHI* GetPlatformRHI();
};