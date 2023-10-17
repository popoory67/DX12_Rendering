#include "Camera.h"

int Camera::Id = -1;

Camera::Camera()
    : UpVector(0.0f, 1.0f, 0.0f)
    , FOV(0.8f)
    , NearPlane(1.0f)
    , FarPlane(1000.0f)
{
    AspectRatio = 100 / 100; // TODO : width / height
}

void Camera::Initialize()
{
    // this placed on world zero position

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
    Position = InPosition;

    float rotation = cosf(InRotation.y);
    {
        ViewVector.x = rotation * sinf(InRotation.z);
        ViewVector.y = sinf(InRotation.y);
        ViewVector.z = rotation * cosf(InRotation.z);
    }
}

void Camera::UpdateView()
{
    ViewMatrix = XMMatrixLookToRH(XMLoadFloat3(&Position), XMLoadFloat3(&ViewVector), XMLoadFloat3(&UpVector));
    ProjectionMatrix = XMMatrixPerspectiveFovRH(FOV, AspectRatio, NearPlane, FarPlane);
}
