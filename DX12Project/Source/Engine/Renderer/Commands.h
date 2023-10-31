#pragma once
#include "Util.h"
#include "MathHelper.h"
#include <vector>

using namespace DirectX;

enum class PipelinePrioirty : unsigned int
{
	PreRender = 0,
	BeginRender,
	DrawSetting,
	Draw,
	EndRender,
	PostRender,
	PriorityMax,
};

class RHICommandContext;

struct RHICommand : public Uncopyable
{
public:
	RHICommand() = default;
	virtual ~RHICommand() = default;
	RHICommand(RHICommand&&) = default;
	RHICommand& operator=(RHICommand&&) = default;

	PipelinePrioirty GetPriority() const { return Priority; }

	virtual void Execute(const RHICommandContext& InContext) = 0;

protected:
	PipelinePrioirty Priority = PipelinePrioirty::PriorityMax;
};

template<class TCommand>
struct RHICommandBase : public RHICommand
{
public:
	RHICommandBase() = default;
	virtual ~RHICommandBase() = default;
	RHICommandBase(RHICommandBase&&) = default;
	RHICommandBase& operator=(RHICommandBase&&) = default;

	template<typename... Args>
	static TCommand* Create(Args&&... args)
	{
		return new TCommand(std::forward<Args>(args)...);
	}
};

#define RHICOMMAND(ClassName) \
struct ClassName : public RHICommandBase<ClassName>

RHICOMMAND(RHICommand_BeginDrawWindow)
{
public:
	RHICommand_BeginDrawWindow() = delete;
	RHICommand_BeginDrawWindow(class RHIViewport* InViewport);
	virtual ~RHICommand_BeginDrawWindow() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	RHIViewport* Viewport;
};

RHICOMMAND(RHICommand_EndDrawWindow)
{
public:
	RHICommand_EndDrawWindow() = delete;
	RHICommand_EndDrawWindow(class RHIViewport* InViewport);
	virtual ~RHICommand_EndDrawWindow() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	RHIViewport* Viewport;
};

RHICOMMAND(RHICommand_BeginRender)
{
public:
	RHICommand_BeginRender() = delete;
	RHICommand_BeginRender(XMMATRIX&& InWorldMatrix);
	virtual ~RHICommand_BeginRender() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	XMMATRIX WorldViewProjection;

	// TODO
	// I need to develop ConstantBuffer process
	_declspec(align(256u)) struct ConstantBuffer
	{
		XMFLOAT4X4 WorldViewProj;
	};
};

RHICOMMAND(RHICommand_SetPrimitive)
{
public:
	RHICommand_SetPrimitive() = delete;
	RHICommand_SetPrimitive(std::vector<struct Vertex>&& InVertexStream, std::vector<unsigned int>&& InIndexStream, unsigned int InStride);
	virtual ~RHICommand_SetPrimitive();

	void Execute(const RHICommandContext& InContext) override;

private:
	std::vector<struct Vertex> VertexStreamResource;
	std::vector<unsigned int> IndexStreamResource;
	unsigned int Stride;
};

RHICOMMAND(RHICommand_DrawPrimitive)
{
public:
	RHICommand_DrawPrimitive() = delete;
	RHICommand_DrawPrimitive(unsigned int InCount); // test parameter
	virtual ~RHICommand_DrawPrimitive() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	unsigned int Count;
	unsigned int Stride;
};