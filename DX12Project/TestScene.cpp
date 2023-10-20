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
    // Entity transform
    {
        OriginalTransform = std::make_shared<TransformComponent>(GetScene());
        AddComponent(OriginalTransform);
        {
            OriginalTransform->SetTransform({ 0.0f, 0.0f, -10.0f }, { 0.0f, 0.0f, 0.0f });
            OriginalTransform->SetScale(0.01f, 0.01f, 0.01f);
            OriginalTransform->SetRotation(45.0f, 0.0f, 0.0f);
        }
    }

    // Primitive
    {
        StaticMesh = std::make_shared<PrimitiveComponent>(GetScene());
        AddComponent(StaticMesh);
        {
            StaticMesh->SetMeshModel(L"../Resources/Obj/diamond.obj");
        }
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

    MoveSpeed = 0.1f;
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
        CameraController = std::make_shared<CameraComponent>(this);
        CameraController->SetPosition(0.0f, 0.0f, 5.0f);
        CameraController->SetRotation(0.0f, 10.0f, 0.0f);
        Object->AddComponent(CameraController);
    }
	Parent::Start();
}

void TestScene::Update()
{


    Parent::Update();
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
