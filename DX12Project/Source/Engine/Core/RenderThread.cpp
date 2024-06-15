#include "RenderThread.h"
#include "Global.h"
#include "Application.h"
#include "ThreadBase.h"
#include "TaskGraph.h"
#include "CommandContext.h"
#include "CommandList.h"
#include "Viewport.h"
#include "SceneRendering.h"
#include "D3D12RenderInterface.h"
#if defined(DEBUG) | defined(_DEBUG)
#include <dxgidebug.h>
#endif

GenericThread* GRenderThread = nullptr;
RHICommandContext GCommandContext;

class RenderWorker : public Task
{
public:
    RenderWorker()
    {

    }

    bool Init() override
    {
        GRHI = RenderInterface::GetPlatformRHI();
        assert(GRHI);

        GRHI->Initialize();
        {
            RHIViewport* viewport = GRHI->CreateViewport(Application::GetWindowHandle(), Application::GetWidth(), Application::GetHeight());
            ViewportRenderer = std::make_unique<Viewport>(viewport);
            ViewportRenderer->Initialize(GetCommandContext());
        }

        Renderer = std::make_unique<SceneRenderer>();
        {
            Renderer->Initialize();
        }
        return true;
    }

    void Run() override
    {
        while (!IsStopped())
        {
            Renderer->BeginRender();

            ViewportRenderer->Draw(GetCommandContext());

            Renderer->Render(GetCommandContext());
            Renderer->EndRender();
        }
    }

private:
    std::unique_ptr<SceneRenderer> Renderer; // 3D
    std::unique_ptr<Viewport> ViewportRenderer; // 2D
};

namespace RenderThread
{
    void StartRenderThread()
    {
        RenderWorker* renderWorker = new RenderWorker();
        GRenderThread = GenericThread::Create(renderWorker, ThreadType::Render);
    }

    void StopRenderThread()
    {
        // End rendering thread
        {
            GRenderThread->Kill();
            SafeDelete(GRenderThread);
        }

        // RHI resources must be cleaned after the rendering thread is done.
        {
            GetCommandContext().CleanUp();
        }

        SafeDelete(GRHI);

#if defined(DEBUG) | defined(_DEBUG)
        ComPtr<IDXGIDebug1> dxgiDebug;
        HRESULT hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
        if (SUCCEEDED(hr))
        {
            dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
            dxgiDebug->Release();
            dxgiDebug = nullptr;
        }
#endif
    }
};