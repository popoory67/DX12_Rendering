#include "TestEntities.h"
#include "InputManager.h"

CameraEntity::CameraEntity(class Scene* InScene)
    : Parent(InScene)
{
    CameraController = std::make_shared<CameraComponent>(GetScene());
    CameraController->SetPosition(0.0f, 0.0f, 3.0f);
    CameraController->SetRotation(0.0f, 0.0f, 0.0f);

    AddComponent(CameraController);

    auto resetCamera = std::bind(&CameraEntity::CameraReset, this);

    auto upCamera = std::bind(&CameraEntity::UpCamera, this);
    InputManager::Get().BindKey('W', upCamera, resetCamera);
    auto downCamera = std::bind(&CameraEntity::DownCamera, this);
    InputManager::Get().BindKey('S', downCamera, resetCamera);
    auto leftCamera = std::bind(&CameraEntity::LeftCamera, this);
    InputManager::Get().BindKey('A', leftCamera, resetCamera);
    auto rightCamera = std::bind(&CameraEntity::RightCamera, this);
    InputManager::Get().BindKey('D', rightCamera, resetCamera);
    auto turnLeftCamera = std::bind(&CameraEntity::TurnLeftCamera, this);
    InputManager::Get().BindKey('Q', turnLeftCamera, resetCamera);
    auto turnRightCamera = std::bind(&CameraEntity::TurnRightCamera, this);
    InputManager::Get().BindKey('E', turnRightCamera, resetCamera);

    auto upScrollCamera = std::bind(&CameraEntity::UpScrollCamera, this);
    InputManager::Get().BindKey(KeyMap::MouseMiddleUp, upScrollCamera, resetCamera);
    auto downScrollCamera = std::bind(&CameraEntity::DownScrollCamera, this);
    InputManager::Get().BindKey(KeyMap::MouseMiddleDown, downScrollCamera, resetCamera);

    MoveSpeed = 0.1f;
}

void CameraEntity::UpCamera()
{
    CameraController->SetRelativePosition(0.0f, MoveSpeed);
}

void CameraEntity::DownCamera()
{
    CameraController->SetRelativePosition(0.0f, -MoveSpeed);
}

void CameraEntity::LeftCamera()
{
    CameraController->SetRelativePosition(-MoveSpeed, 0.0f);
}

void CameraEntity::RightCamera()
{
    CameraController->SetRelativePosition(MoveSpeed, 0.0f);
}

// TODO
// The rotation of camera didn't apply the camera transform.
void CameraEntity::TurnLeftCamera()
{
    CameraController->SetRelativeRotation(0.0f, 0.0f, -MoveSpeed * 5.0f);
}

void CameraEntity::TurnRightCamera()
{
    CameraController->SetRelativeRotation(0.0f, 0.0f, MoveSpeed * 5.0f);
}

void CameraEntity::UpScrollCamera()
{
    CameraController->SetRelativePosition(0.0f, 0.0f, -MoveSpeed);
}

void CameraEntity::DownScrollCamera()
{
    CameraController->SetRelativePosition(0.0f, 0.0f, MoveSpeed);
}

void CameraEntity::CameraReset()
{
    CameraController->SetRelativePosition(0.0f, 0.0f);
}

ModelEntity::ModelEntity(class Scene* InScene)
    : Parent(InScene)
{
    // Entity transform
    {
        OriginalTransform = std::make_shared<TransformComponent>(GetScene());
        AddComponent(OriginalTransform);
    }
    // Material
    {
        Material = std::make_shared<MaterialComponent>(GetScene());
        AddComponent(Material);
    }
    // Primitive
    {
        StaticMesh = std::make_shared<MeshComponent>(GetScene());
        StaticMesh->SetMaterialComponent(Material);
        AddComponent(StaticMesh);
    }
}

void ModelEntity::SetStaticMesh(const std::wstring& InPath)
{
    StaticMesh->SetMeshModel(InPath);
}

void ModelEntity::SetPosition(float InX, float InY, float InZ /*= 0.0f*/)
{
    OriginalTransform->SetPosition(InX, InY, InZ);
}

void ModelEntity::SetRotation(float InRoll, float InPitch, float InYaw)
{
    OriginalTransform->SetRotation(InRoll, InPitch, InYaw);
}

void ModelEntity::SetScale(float InX, float InY, float InZ)
{
    OriginalTransform->SetScale(InX, InY, InZ);
}
