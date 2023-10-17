#include "TestScene.h"
#include "Entity.h"
#include "PrimitiveComponent.h"
#include "CameraComponent.h"
#include "InputManager.h"

TestScene::TestScene()
{
    auto upCamera = std::bind(&TestScene::UpCamera, this);
    InputManager::Get().BindKey('W', upCamera);
    auto downCamera = std::bind(&TestScene::DownCamera, this);
    InputManager::Get().BindKey('S', downCamera);
    auto leftCamera = std::bind(&TestScene::LeftCamera, this);
    InputManager::Get().BindKey('A', leftCamera);
    auto rightCamera = std::bind(&TestScene::RightCamera, this);
    InputManager::Get().BindKey('D', rightCamera);
}

TestScene::~TestScene()
{

}

void TestScene::Start()
{
	Object = std::make_shared<Entity>();

    AddEntity(Object);

    // Primitive
    {
        std::shared_ptr<PrimitiveComponent> primitiveData = std::make_shared<PrimitiveComponent>(this, nullptr);
        primitiveData->SetMeshModel(L"../Resources/Obj/diamond.obj");
        Object->AddComponent(primitiveData);
    }

    // Camera
    {
        CameraController = std::make_shared<CameraComponent>(this, nullptr);
        CameraController->SetTransform({ 0.0f, -2.0f, 0.0f }, {});
        Object->AddComponent(CameraController);
    }

	Parent::Start();
}

void TestScene::UpCamera()
{
    CameraController->SetRelativePosition(0.0f, -20.0f, 0.0f);
}

void TestScene::DownCamera()
{
    CameraController->SetRelativePosition(0.0f, 20.0f, 0.0f);
}

void TestScene::LeftCamera()
{
    CameraController->SetRelativePosition(-20.0f, 0.0f, 0.0f);
}

void TestScene::RightCamera()
{
    CameraController->SetRelativePosition(20.0f, 0.0f, 0.0f);
}
