#include "stdafx.h"
#include "Viewport.h"
#include "CommandList.h"

Viewport::Viewport(RHIViewport*&& InViewportRHI)
	: ViewportInterface(InViewportRHI)
{

}

Viewport::~Viewport()
{
	SafeDelete(ViewportInterface);
}

void Viewport::Draw(RHICommandList& InCommandList)
{
	InCommandList.BeginDrawWindow(ViewportInterface);

	InCommandList.EndDrawWindow(ViewportInterface);
}

// TODO
// The test codes that split Viewport::Draw to verify the order of that function are listed below.
// These need to be rearranged like Viewport::Draw.
void Viewport::BeginDrawWindow(RHICommandList& InCommandList)
{
	InCommandList.BeginDrawWindow(ViewportInterface);
}

void Viewport::EndDrawWindow(RHICommandList& InCommandList)
{
	InCommandList.EndDrawWindow(ViewportInterface);
}
