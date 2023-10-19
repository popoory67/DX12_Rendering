#include "CameraComponent.h"
#include "Scene.h"
#include "Camera.h"

CameraComponent::CameraComponent(Scene* InScene, Component* InParent)
    : Parent(InScene, InParent)
    , TransformComponent(this)
    , FOV(3.14159265359f / 4.0f) // 45 degrees
    , AspectRatio(1.778f)
{
    const Camera* camera = InScene->GetMainCamera();
    CameraId = camera->GetId();

    WorldPosition = { 1.0f, -1.0f, 8.0f };
}

void CameraComponent::SetFOV(float InFOV)
{
    FOV = InFOV;
}

void CameraComponent::SetAspectRatio(float InAspectRatio)
{
    AspectRatio = InAspectRatio;
}

void CameraComponent::SetAspectRatio(float InWidth, float InHeight)
{
    AspectRatio = InWidth / InHeight;
}

void CameraComponent::Update()
{
    Scene* currentScene = GetScene();
    assert(currentScene);

    if (Camera* camera = currentScene->GetCamera(CameraId))
    {
        camera->SetTransform(WorldPosition, Rotation);
        camera->SetFOV(FOV);
        camera->SetAspectRatio(AspectRatio);

        camera->UpdateView();
    }

    Parent::Update();
}
