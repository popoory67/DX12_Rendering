#include "RenderInterface.h"

RHI* GRHI = nullptr;

// TODO
// test
#include "D3D12RenderInterface.h"
using ProcessPlatform = D3D12RHI;

namespace RenderInterface
{
    RHI* GetPlatformRHI()
    {
        return new ProcessPlatform();
    }
};
