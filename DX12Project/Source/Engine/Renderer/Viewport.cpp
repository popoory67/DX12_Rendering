#include "Util.h"
#include "Viewport.h"
#include "CommandContext.h"
#include "CommandList.h"

Viewport::Viewport(RHIViewport*&& InViewportRHI)
	: ViewportInterface(std::move(InViewportRHI))
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
	{
        RHICommand_BeginDrawWindow* viewportCommand = new RHICommand_BeginDrawWindow(ViewportInterface);

        InContext.AddCommand(std::move(viewportCommand));
	}

	//InContext.GetCommandList().BeginRender();

    {
		RHICommand_EndDrawWindow* viewportCommand = new RHICommand_EndDrawWindow(ViewportInterface);

        InContext.AddCommand(std::move(viewportCommand));
    }
}