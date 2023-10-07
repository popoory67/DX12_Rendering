#include "GameThread.h"
#include "ThreadBase.h"
#include "Global.h"
#include "TaskGraph.h"

#include "../TestScene.h" // dummy

GenericThread* GGameThread = nullptr;

class GameWorker : public Task
{
public:
    GameWorker(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition)
        : Mutex(InMutex)
        , Condition(InCondition)
    {
    }

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
        while (!bStop)
        {
            std::unique_lock<std::mutex> lock(*Mutex);
            Condition->wait(lock, []()
            {
                return TaskGraphSystem::Get().GetThreadState() == ThreadType::Main;
            });

            TaskGraphSystem::Get().Execute(ThreadType::Main);

            CurrentScene->Update();

            Condition->notify_all();
        }
    }

    void Stop() override
    {
        bStop = true;
        CurrentScene->End();

        TaskGraphSystem::Get().SetThreadState(ThreadType::Main); // test

        Condition->notify_all();
    }

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

    bool bStop = false;

    std::shared_ptr<std::mutex> Mutex;
    std::shared_ptr<std::condition_variable> Condition;
};

namespace GameThread
{
    void StartGameThread(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition)
    {
        GameWorker* gameWorker = new GameWorker(InMutex, InCondition);
        GGameThread = GenericThread::Create(gameWorker, ThreadType::Main);
    }

    void StopGameThread()
    {
        GGameThread->Kill();

        SafeDelete(GGameThread);
    }
};