#include "TransformComponent.h"

TransformComponent::TransformComponent(Component* InChild)
    : WorldPosition(0.0f, 0.0f, 0.0f)
    , Rotation(0.0f, 0.0f, 0.0f)
    , Scale(1.0f, 1.0f, 1.0f)
    , Child(InChild)
{

}

void TransformComponent::SetRelativeTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation, const XMFLOAT3& InScale /*= { 1.0f, 1.0f, 1.0f }*/)
{
    SetRelativePosition(InPosition.x, InPosition.y, InPosition.z);
}

void TransformComponent::SetRelativePosition(float InX, float InY, float InZ)
{
    WorldPosition.x += InX;
    WorldPosition.y += InY;
    WorldPosition.z += InZ;
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