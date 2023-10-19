#include "TestScene.h"
#include "Entity.h"
#include "PrimitiveComponent.h"
#include "CameraComponent.h"
#include "InputManager.h"

Model::Model(class Scene* InScene)
    : Parent(InScene)
{

}

void Model::Initialize()
{
    // Primitive
    {
        StaticMesh = std::make_shared<PrimitiveComponent>(GetScene(), nullptr);
        StaticMesh->SetMeshModel(L"../Resources/Obj/diamond.obj");
        StaticMesh->SetScale(0.1f, 0.1f, 0.1f);
        AddComponent(StaticMesh);
    }
}

TestScene::TestScene()
{
    auto resetCamera = std::bind(&TestScene::CameraReset, this);

    auto upCamera = std::bind(&TestScene::UpCamera, this);
    InputManager::Get().BindKey('W', upCamera, resetCamera);
    auto downCamera = std::bind(&TestScene::DownCamera, this);
    InputManager::Get().BindKey('S', downCamera, resetCamera);
    auto leftCamera = std::bind(&TestScene::LeftCamera, this);
    InputManager::Get().BindKey('A', leftCamera, resetCamera);
    auto rightCamera = std::bind(&TestScene::RightCamera, this);
    InputManager::Get().BindKey('D', rightCamera, resetCamera);
    auto upScrollCamera = std::bind(&TestScene::UpScrollCamera, this);
    InputManager::Get().BindKey(KeyMap::MouseMiddleUp, upScrollCamera, resetCamera);
    auto downScrollCamera = std::bind(&TestScene::DownScrollCamera, this);
    InputManager::Get().BindKey(KeyMap::MouseMiddleDown, downScrollCamera, resetCamera);

    MoveSpeed = 10.0f;
}

TestScene::~TestScene()
{

}

void TestScene::Start()
{
	Object = std::make_shared<Model>(this);
    AddEntity(Object);

    // Camera
    {
        CameraController = std::make_shared<CameraComponent>(this, nullptr);
        CameraController->SetPosition(1.0f, -1.0f, 428.0f);
        Object->AddComponent(CameraController);
    }
	Parent::Start();
}

void TestScene::UpCamera()
{
    CameraController->SetRelativePosition(0.0f, MoveSpeed);
}

void TestScene::DownCamera()
{
    CameraController->SetRelativePosition(0.0f, -MoveSpeed);
}

void TestScene::LeftCamera()
{
    CameraController->SetRelativePosition(-MoveSpeed, 0.0f);
}

void TestScene::RightCamera()
{
    CameraController->SetRelativePosition(MoveSpeed, 0.0f);
}

void TestScene::UpScrollCamera()
{
    CameraController->SetRelativePosition(0.0f, 0.0f, -MoveSpeed);
}

void TestScene::DownScrollCamera()
{
    CameraController->SetRelativePosition(0.0f, 0.0f, MoveSpeed);
}

void TestScene::CameraReset()
{
    CameraController->SetRelativePosition(0.0f, 0.0f);
}
