#include <3d/s3Camera.h>

s3Camera::s3Camera(t3Vector3f origin, t3Vector3f lookAt, t3Vector3f up, float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    setCameraToWorld(origin, lookAt, up);
    setProjectionMatrix(aspectRatio, fovDeg, nearZ, farZ);
}

s3Camera::~s3Camera()
{
}

void s3Camera::setProjectionMatrix(float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    this->aspectRatio = aspectRatio;
    this->fov = fovDeg;
    this->nearZ = nearZ;
    this->farZ = farZ;

    float halfFov = t3Math::Deg2Rad(fov / 2.0f);
    float tanHalfFov = t3Math::sinRad(halfFov) / t3Math::cosRad(halfFov);
    float rangeZ = farZ - nearZ;
    float A = farZ / rangeZ, B = -nearZ * farZ / rangeZ;

    projectionMatrix._mat[0][0] = 1.0f / (aspectRatio * tanHalfFov);
    projectionMatrix._mat[0][1] = 0.0f;
    projectionMatrix._mat[0][2] = 0.0f;
    projectionMatrix._mat[0][3] = 0.0f;

    projectionMatrix._mat[1][0] = 0.0f;
    projectionMatrix._mat[1][1] = 1.0f / tanHalfFov;
    projectionMatrix._mat[1][2] = 0.0f;
    projectionMatrix._mat[1][3] = 0.0f;

    projectionMatrix._mat[2][0] = 0.0f;
    projectionMatrix._mat[2][1] = 0.0f;
    projectionMatrix._mat[2][2] = A;
    projectionMatrix._mat[2][3] = B;

    projectionMatrix._mat[3][0] = 0.0f;
    projectionMatrix._mat[3][1] = 0.0f;
    projectionMatrix._mat[3][2] = 1.0f;
    projectionMatrix._mat[3][3] = 0.0f;
}

t3Matrix4x4 s3Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

void s3Camera::setCameraToWorld(t3Vector3f origin, t3Vector3f lookAt, t3Vector3f cameraUp)
{
    t3Vector3f direction = (lookAt - origin).getNormalized();
    t3Vector3f right = cameraUp.getCrossed(direction).getNormalized();
    t3Vector3f up = direction.getCrossed(right).getNormalized();;
    cameraToWorld.set(right.x, up.x, direction.x, origin.x,
                      right.y, up.y, direction.y, origin.y,
                      right.z, up.z, direction.z, origin.z,
                      0, 0, 0, 1);
    worldToCamera = cameraToWorld.getInverse();
}

t3Matrix4x4 s3Camera::getCameraToWorld() const
{
    return cameraToWorld;
}

t3Matrix4x4 s3Camera::getWorldToCamera() const
{
    return worldToCamera;
}

float32 s3Camera::getAspectRatio() const
{
    return aspectRatio;
}

float32 s3Camera::getNearZ() const
{
    return nearZ;
}

float32 s3Camera::getFarZ() const
{
    return farZ;
}

float32 s3Camera::getFov() const
{
    return fov;
}
