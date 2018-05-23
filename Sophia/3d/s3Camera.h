#pragma once

#include <core/s3Settings.h>
#include <t3Vector3.h>
#include <t3Matrix4x4.h>

class s3Camera
{
public:
    s3Camera(t3Vector3f origin, t3Vector3f lookAt, t3Vector3f up, 
             float32 aspectRatio = 1.333f, float32 fovDeg = 45.0f, float32 nearZ = 0.1f, float32 farZ = 100.0f);
    ~s3Camera();

    void setProjectionMatrix(float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ);
    t3Matrix4x4 getProjectionMatrix() const;

    void setCameraToWorld(t3Vector3f origin, t3Vector3f lookAt, t3Vector3f up);
    t3Matrix4x4 getCameraToWorld() const;
    t3Matrix4x4 getWorldToCamera() const;
    void getViewAxis(t3Vector3f& origin, t3Vector3f& right, t3Vector3f& up, t3Vector3f& direction);

    float32 getAspectRatio() const;
    float32 getNearZ() const;
    float32 getFarZ() const;
    float32 getFov() const;

private:
    void handleInit();
    void handleDeinit();

    // degree
    float32 fov;
    float32 aspectRatio;
    float32 nearZ, farZ;

    t3Matrix4x4 cameraToWorld, worldToCamera;
    t3Matrix4x4 projectionMatrix;

    t3Vector3f origin, lookAt, up;

    class s3CameraHandle;
    s3CameraHandle* cameraHandle;
};
