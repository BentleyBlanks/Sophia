#pragma once

#include <core/s3Settings.h>
#include <t3Vector3.h>
#include <t3Matrix4x4.h>

class s3Camera
{
public:
    s3Camera(t3Vector3f origin, t3Vector3f direction, t3Vector3f up, 
             float32 aspectRatio = 1.333f, float32 fovDeg = 45.0f, float32 nearZ = 0.1f, float32 farZ = 100.0f);
    ~s3Camera();

    void setProjectionMatrix(float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ);
    t3Matrix4x4 getProjectionMatrix() const;

    void setCameraToWorld(const t3Vector3f& origin, const t3Vector3f& direction, const t3Vector3f& up);
    void setCameraToWorld(const t3Matrix4x4& cameraToWorld);
    t3Matrix4x4 getCameraToWorld() const;
    t3Matrix4x4 getWorldToCamera() const;

    t3Vector3f getOrigin() const;
    t3Vector3f getUpAxis() const;
    t3Vector3f getDirectionAxis() const;
    t3Vector3f getRightAxis() const;
    void getViewAxis(t3Vector3f& origin, t3Vector3f& right, t3Vector3f& up, t3Vector3f& direction) const;

    float32 getAspectRatio() const;
    float32 getNearZ() const;
    float32 getFarZ() const;
    float32 getFovY() const;

    // mouse / keyboard control
    void setMouseSpeed(float mouseSpeed);
    void setKeyboardSpeed(float keyboardSpeed);

    void setMouseEventState(bool state);
    void setKeyboardEventState(bool state);
    bool getMouseEventState() const;
    bool getKeyboardEventState() const;
private:
    void handleInit();
    void handleDeinit();

    // degree
    float32 fovY;
    float32 aspectRatio;
    float32 nearZ, farZ;

    t3Matrix4x4 cameraToWorld, worldToCamera;
    t3Matrix4x4 projectionMatrix;

    class s3CameraHandle;
    s3CameraHandle* cameraHandle;

    // Event
    bool mouseEventState, keyboardEventState;
};
