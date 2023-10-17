#include "Util.h"
#include "Viewport.h"
#include "CommandContext.h"
#include "CommandList.h"

Viewport::Viewport(RHIViewport* InViewportRHI)
	: ViewportInterface(InViewportRHI)
{

}

Viewport::~Viewport()
{
	SafeDelete(ViewportInterface);
}

void Viewport::Initialize(RHICommandContext& InContext)
{
	InContext.GetCommandList().ResizeViewport(ViewportInterface);
}

void Viewport::Draw(RHICommandContext& InContext)
{
	RHICommand_BeginDrawWindow* beginDraw = new RHICommand_BeginDrawWindow(ViewportInterface);
	{
        InContext.AddCommand(std::move(beginDraw));
	}

	RHICommand_EndDrawWindow* endDraw = new RHICommand_EndDrawWindow(ViewportInterface);
    {
        InContext.AddCommand(std::move(endDraw));
    }
}