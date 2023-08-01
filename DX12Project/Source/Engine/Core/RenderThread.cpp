#include "RenderThread.h"
#include "Global.h"
#include "ThreadBase.h"
#include "TaskGraph.h"
#include "Application.h"
#include "CommandList.h"
#include "SceneRendering.h"
#include "Viewport.h"
#include "D3D12RenderInterface.h"

// dummy
#include "../TestScene.h"

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
        if (RHIViewport* viewport = rhi->CreateViewport(Application::GetWindowHandle(), 100, 100/*MainWindowHandle, ClientWidth, ClientHeight*/))
        {
            ViewportRenderer = std::make_unique<Viewport>(std::move(viewport));
        }

        // create scene renderer
        Renderer = std::make_unique<SceneRenderer>(rhi);
        if (Renderer)
        {
            if (Renderer->Initialize())
            {
                // Create dummy scene
                TestScene* test = new TestScene();
                Renderer->AddScene(test);
                Renderer->SetCurrentScene(test->GetSceneId());
            }
        }

        // TODO
        // Load assets, PSO(on the other thread)

        return true;
    }

    void Run() override
    {
        Renderer->BeginRender();

        ViewportRenderer->BeginDrawWindow(GCommandContext.GetCommandList());

        // GCommandContext has to be managed in the render thread, and the thread is the only place we can access it.
        //ViewportRenderer->Draw(GCommandContext.GetCommandList());

        Renderer->Render(GCommandContext.GetCommandList());

        Renderer->EndRender();
        ViewportRenderer->EndDrawWindow(GCommandContext.GetCommandList());
    }

    void Stop() override
    {

    }

private:
    // test
    std::unique_ptr<class SceneRenderer> Renderer; // 3D
    std::unique_ptr<class Viewport> ViewportRenderer; // 2D
};

namespace RenderThread
{
    void StartRenderThread()
    {
        GRenderWorker = new RenderWorker();

        GRenderThread = GenericThread::Create(GRenderWorker);

    }

    void StopRenderThread()
    {
        GRenderWorker = nullptr;
        GRenderThread = nullptr;
    }
};
