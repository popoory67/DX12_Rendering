#pragma once
#include "Util.h"
#include "MathHelper.h"
#include "CommandContext.h"
#include <vector>

using namespace DirectX;

enum class CommandPriority
{
	First = 0,
	Last = 2,
};

struct RHICommand : public Uncopyable
{
public:
	RHICommand() = default;
	virtual ~RHICommand() = default;
	RHICommand(RHICommand&&) = default;
	RHICommand& operator=(RHICommand&&) = default;

	CommandPriority GetPriority() const { return Priority; }

	virtual void Execute(const RHICommandContext& InContext) = 0;

protected:
	CommandPriority Priority = CommandPriority::Last;
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

struct RHICommand_BeginDrawWindow : public RHICommandBase<RHICommand_BeginDrawWindow>
{
	RHICommand_BeginDrawWindow() = delete;
	RHICommand_BeginDrawWindow(class RHIViewport* InViewport);
	virtual ~RHICommand_BeginDrawWindow() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	RHIViewport* Viewport;
};

struct RHICommand_EndDrawWindow : public RHICommandBase<RHICommand_EndDrawWindow>
{
	RHICommand_EndDrawWindow() = delete;
	RHICommand_EndDrawWindow(class RHIViewport* InViewport);
	virtual ~RHICommand_EndDrawWindow() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	RHIViewport* Viewport;
};

struct RHICommand_BeginRender : public RHICommandBase<RHICommand_BeginRender>
{
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

struct RHICommand_SetRenderTargets : public RHICommandBase<RHICommand_SetRenderTargets>
{
	RHICommand_SetRenderTargets() = delete;
	RHICommand_SetRenderTargets(class RHIRenderTargetInfo* InRenderTargetViews, unsigned int InNumRenderTargets, class RHIResource* InRenderTargetView);
	virtual ~RHICommand_SetRenderTargets();

	void Execute(const RHICommandContext& InContext) override;

private:
	RHIRenderTargetInfo* RenderTargets;
	unsigned int NumRenderTargets;
	RHIResource* DepthStencil;
};

struct RHICommand_SetPrimitive : public RHICommandBase<RHICommand_SetPrimitive>
{
	RHICommand_SetPrimitive() = delete;
	RHICommand_SetPrimitive(std::vector<struct Vertex>&& InVertexStream, std::vector<unsigned int>&& InIndexStream, unsigned int InSize, unsigned int InStride);
	virtual ~RHICommand_SetPrimitive();

	void Execute(const RHICommandContext& InContext) override;

private:
	std::vector<struct Vertex> VertexStreamResource;
	std::vector<unsigned int> IndexStreamResource;
	unsigned int Size;
	unsigned int Stride;
};

struct RHICommand_DrawPrimitive : public RHICommandBase<RHICommand_DrawPrimitive>
{
	RHICommand_DrawPrimitive() = delete;
	RHICommand_DrawPrimitive(unsigned int InCount); // test parameter
	virtual ~RHICommand_DrawPrimitive() = default;

	void Execute(const RHICommandContext& InContext) override;

private:
	unsigned int Count;
	unsigned int Stride;
};