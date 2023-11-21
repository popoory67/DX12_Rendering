#include "Camera.h"

int Camera::Id = -1;

Camera::Camera()
    : FOV(MathHelper::Pi / 4.0f) // 45 degrees
    , AspectRatio(1.778f)
    , NearPlane(1.0f)
    , FarPlane(1000.0f)
{
    Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    ViewVector = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
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

    const float radian = 3.14f / 180;

    // TODO
    // Rotation have to apply Camera transform.
    float rotation = cosf(InRotation.y * radian);
    {
        float roll = rotation * sinf(InRotation.z * radian);
        float pitch = sinf(InRotation.y * radian);
        float yaw = rotation * cosf(InRotation.z * radian);

        ViewVector = XMVectorSet(roll, pitch, yaw + InPosition.z, 0.0f);
    }
}

void Camera::UpdateView()
{
    ViewMatrix = XMMatrixLookAtLH(Position, ViewVector, UpVector);
    ProjectionMatrix = XMMatrixPerspectiveFovRH(FOV, AspectRatio, NearPlane, FarPlane);
}
