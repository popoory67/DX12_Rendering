#include "TransformComponent.h"

TransformComponent::TransformComponent(Scene* InScene, Component* InParent)
    : Parent(InScene, InParent)
    , WorldPosition(0.0f, 0.0f, 0.0f)
    , Rotation(0.0f, 0.0f, 0.0f)
    , Scale(1.0f, 1.0f, 1.0f)
{

}

DirectX::XMFLOAT3 TransformComponent::GetPosition() const
{
    return WorldPosition;
}

DirectX::XMFLOAT3 TransformComponent::GetRotation() const
{
    return Rotation;
}

DirectX::XMFLOAT3 TransformComponent::GetScale() const
{
    return Scale;
}

void TransformComponent::SetRelativeTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale /*= { 1.0f, 1.0f, 1.0f }*/)
{
    SetRelativePosition(InPosition.x, InPosition.y, InPosition.z);
    SetRelativeRotation(InRotation.x, InRotation.y, InRotation.z);
}

void TransformComponent::SetRelativePosition(float InX, float InY, float InZ)
{
    WorldPosition.x += InX;
    WorldPosition.y += InY;
    WorldPosition.z += InZ;
}

void TransformComponent::SetRelativeRotation(float InX, float InY, float InZ /*= 0.0f*/)
{
    Rotation.x += InX;
    Rotation.y += InY;
    Rotation.z += InZ;
}

void TransformComponent::SetTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale)
{
    WorldPosition = InPosition;
    Rotation = InRotation;
    Scale = InScale;
}

void TransformComponent::SetPosition(float InX, float InY, float InZ)
{
    WorldPosition.x = InX;
    WorldPosition.y = InY;
    WorldPosition.z = InZ;
}

void TransformComponent::SetRotation(float InRoll, float InPitch, float InYaw)
{
    Rotation.x = InRoll;
    Rotation.y = InPitch;
    Rotation.z = InYaw;
}

void TransformComponent::SetScale(float InX, float InY, float InZ)
{
    Scale.x = InX;
    Scale.y = InY;
    Scale.z = InZ;
}