#include "RenderThread.h"
#include "Global.h"
#include "Application.h"
#include "ThreadBase.h"
#include "TaskGraph.h"
#include "CommandList.h"
#include "Viewport.h"
#include "SceneRendering.h"
#include "D3D12RenderInterface.h"
#include <dxgidebug.h>

GenericThread* GRenderThread = nullptr;
Task* GRenderWorker = nullptr;

class RenderWorker : public Task
{
public:
    bool Init() override
    {
        std::shared_ptr<D3D12RHI> rhi = std::make_shared<D3D12RHI>();
        rhi->Initialize();

        // create viewport
        RHIViewport* viewport = rhi->CreateViewport(Application::GetWindowHandle(), 100, 100/*MainWindowHandle, ClientWidth, ClientHeight*/);
        ViewportRenderer = std::make_unique<Viewport>(std::move(viewport));

        // create scene renderer
        Renderer = std::make_unique<SceneRenderer>(rhi);
        Renderer->Initialize();

        // TODO
        // Load assets, PSO(on the other thread)

        return true;
    }

    void Run() override
    {
        while (!bStop)
        {
            // TODO
            // test
            TaskGraphSystem::Get().Execute(ThreadType::Render);

            Renderer->BeginRender();

            ViewportRenderer->BeginDrawWindow(GCommandContext.GetCommandList());

            // GCommandContext has to be managed in the render thread, and the thread is the only place we can access it.
            //ViewportRenderer->Draw(GCommandContext.GetCommandList());

            Renderer->Render(GCommandContext.GetCommandList());

            Renderer->EndRender();
            ViewportRenderer->EndDrawWindow(GCommandContext.GetCommandList());
        }
    }

    void Stop() override
    {
        bStop = true;
    }

private:
    // test
    std::unique_ptr<class SceneRenderer> Renderer; // 3D
    std::unique_ptr<class Viewport> ViewportRenderer; // 2D

    bool bStop = false;
};

namespace RenderThread
{
    void StartRenderThread()
    {
        GRenderWorker = new RenderWorker();

        GRenderThread = GenericThread::Create(GRenderWorker, ThreadType::Render);

    }

    void StopRenderThread()
    {
        GRenderThread->Kill();

        SafeDelete(GRenderThread);
        GRenderWorker = nullptr;

#if _DEBUG
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

void RenderCommand::DoTask()
{
    Func(GCommandContext.GetCommandList());
}
