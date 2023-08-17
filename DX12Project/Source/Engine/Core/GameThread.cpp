#include "GameThread.h"
#include "ThreadBase.h"
#include "Global.h"
#include "TaskGraph.h"

#include "../TestScene.h" // dummy

GenericThread* GGameThread = nullptr;
Task* GGameWorker = nullptr;

class GameWorker : public Task
{
public:
    bool Init() override
    {
        std::shared_ptr<TestScene> test = std::make_shared<TestScene>();
        test->Start();

        int sceneId = AddScene(std::move(test));
        SetCurrentScene(sceneId);

        return true;
    }

    void Run() override
    {
        // TODO
        // test
        TaskGraphSystem::Get().Execute(ThreadType::Main);
    }

    void Stop() override
    {

    }

    // TODO
    // Scene management
private:
    std::shared_ptr<Scene> GetCurrentScene()
    {
        return CurrentScene;
    }

    int AddScene(std::shared_ptr<TestScene>&& InScene)
    {
        assert(InScene);

        InScene->SetSceneId(IndexCount);
        SceneList.emplace(std::make_pair(IndexCount, std::move(InScene)));

        int index = IndexCount;
        ++IndexCount;
        return index;
    }

    void SetCurrentScene(int InIndex)
    {
        assert(SceneList[InIndex]);

        CurrentSceneIndex = InIndex;
        CurrentScene = SceneList[InIndex];
    }

private:
    std::unordered_map<unsigned int, std::shared_ptr<class Scene>> SceneList;
    unsigned int IndexCount = 0;
    int CurrentSceneIndex = INVALID_INDEX;

    std::shared_ptr<class Scene> CurrentScene;
};

namespace GameThread
{
    void StartGameThread()
    {
        GGameWorker = new GameWorker();
        GGameThread = GenericThread::Create(GGameWorker, ThreadType::Main);
    }

    void StopGameThread()
    {
        GGameThread->Kill();

        SafeDelete(GGameThread);
        GGameWorker = nullptr;
    }
};