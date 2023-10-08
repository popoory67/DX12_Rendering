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
    RenderWorker(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition)
        : Mutex(InMutex)
        , Condition(InCondition)
    {

    }

    bool Init() override
    {
        GRHI = RenderInterface::GetPlatformRHI();
        assert(GRHI);

        GRHI->Initialize();
        {
            RHIViewport* viewport = GRHI->CreateViewport(Application::GetWindowHandle(), 100, 100/*ClientWidth, ClientHeight*/);
            ViewportRenderer = std::make_unique<Viewport>(std::move(viewport));
            ViewportRenderer->Initialize(GCommandContext);
        }

        //Renderer = std::make_unique<SceneRenderer>();
        //{
        //    Renderer->Initialize();
        //}
        return true;
    }

    void Run() override
    {
        while (!bStop)
        {
            std::unique_lock<std::mutex> lock(*Mutex);
            Condition->wait(lock, []()
            {
                return TaskGraphSystem::Get().GetThreadState() == ThreadType::Render;
            });

            TaskGraphSystem::Get().Execute(ThreadType::Render);

            Renderer->BeginRender();

            ViewportRenderer->Draw(GCommandContext);

            Renderer->Render(GCommandContext);
            Renderer->EndRender();

            Condition->notify_all();
        }
    }

    void Stop() override
    {
        bStop = true;
        Condition->notify_all();
    }

private:
    std::unique_ptr<SceneRenderer> Renderer; // 3D
    std::unique_ptr<Viewport> ViewportRenderer; // 2D

    bool bStop = false;

    std::shared_ptr<std::mutex> Mutex;
    std::shared_ptr<std::condition_variable> Condition;
};

namespace RenderThread
{
    void StartRenderThread(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition)
    {
        RenderWorker* renderWorker = new RenderWorker(InMutex, InCondition);
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
            GCommandContext.CleanUp();
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

RenderCommand::~RenderCommand()
{

}

void RenderCommand::DoTask()
{
    Func(GCommandContext);
}
