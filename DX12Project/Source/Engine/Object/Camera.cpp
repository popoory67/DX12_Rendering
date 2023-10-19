#include "Camera.h"

int Camera::Id = -1;

Camera::Camera()
    : FOV(3.14159265359f / 4.0f) // 45 degrees
    , AspectRatio(1.778f)
    , NearPlane(1.1f)
    , FarPlane(1000.0f)
{
    Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    ViewVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    ViewMatrix = XMMatrixLookAtLH(Position, ViewVector, UpVector);

    ++Id;
}

int Camera::GetId() const
{
    return Id;
}

XMMATRIX Camera::GetViewMatrix() const
{
    return ViewMatrix;
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    return ProjectionMatrix;
}

void Camera::SetFOV(float InFOV)
{
    FOV = InFOV;
}

void Camera::SetAspectRatio(float InAspectRatio)
{
    AspectRatio = InAspectRatio;
}

void Camera::SetPlane(float InNear, float InFar)
{
    NearPlane = InNear;
    FarPlane = InFar;
}

void Camera::SetTransform(const XMFLOAT3& InPosition, const XMFLOAT3& InRotation)
{
    Position = XMVectorSet(InPosition.x, InPosition.y, InPosition.z, 0.0f);

    float rotation = cosf(InRotation.y);
    {
        float roll = rotation * sinf(InRotation.z);
        float pitch = sinf(InRotation.y);
        float yaw = rotation * cosf(InRotation.z);

        ViewVector += XMVectorSet(roll, pitch, yaw, 0.0f);
    }
}

void Camera::UpdateView()
{
    ViewMatrix = XMMatrixLookAtLH(Position, ViewVector, UpVector);
    ProjectionMatrix = XMMatrixPerspectiveFovRH(FOV, AspectRatio, NearPlane, FarPlane);
}
