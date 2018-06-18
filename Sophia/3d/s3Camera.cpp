#include <3d/s3Camera.h>
#include <core/s3Callback.h>
#include <core/s3Event.h>
#include <app/s3CallbackManager.h>
#include <core/s3MathHepler.h>

class s3Camera::s3CameraHandle : public s3CallbackHandle
{
public:
    s3CameraHandle(s3Camera* camera) 
        : 
        camera(camera), 
        keyboardSpeed(0.1f), 
        mouseSpeed(0.1f), 
        angleX(90.0f),
        angleY(0.0f) {}

    void onHandle(const s3CallbackUserData* data)
    {
        t3Vector3f origin, right, up, direction;
        camera->getViewAxis(origin, right, up, direction);

        if (camera->keyboardEventState)
        {
            if (data->sender == &s3CallbackManager::callBack.onKeyPressed)
            {
                s3KeyEvent* keyEvent = (s3KeyEvent*)data->userData;
                if (keyEvent->key == 'w')
                    origin += keyboardSpeed * direction;
                else if (keyEvent->key == 'a')
                    origin += keyboardSpeed * -right;
                else if (keyEvent->key == 's')
                    origin += keyboardSpeed * -direction;
                else if (keyEvent->key == 'd')
                    origin += keyboardSpeed * right;

                if (keyEvent->keyCode == s3KeyCode::Up)
                    origin += keyboardSpeed * up;
                else if (keyEvent->keyCode == s3KeyCode::Down)
                    origin += keyboardSpeed * -up;
                else if (keyEvent->keyCode == s3KeyCode::Left)
                    origin += keyboardSpeed * -right;
                else if (keyEvent->keyCode == s3KeyCode::Right)
                    origin += keyboardSpeed * right;
#ifdef TEST
                keyEvent->print(0);
#endif
            }
            else if (data->sender == &s3CallbackManager::callBack.onKeyReleased)
            {
                s3KeyEvent* keyEvent = (s3KeyEvent*)data->userData;
#ifdef TEST
                keyEvent->print(1);
#endif
            }
        }

        if (camera->mouseEventState)
        {
            if (data->sender == &s3CallbackManager::callBack.onMouseMoved)
            {
                s3MouseEvent* mouseEvent = (s3MouseEvent*)data->userData;

                if (mouseEvent->type == s3MouseEvent::s3ButtonType::LEFT)
                {
                    float32 offsetX = mouseEvent->offsetX * mouseSpeed;
                    float32 offsetY = mouseEvent->offsetY * mouseSpeed;

                    angleX += offsetX;
                    angleY += offsetY;

                    if (angleY > 89.0f)
                        angleY = 89.0f;
                    if (angleY < -89.0f)
                        angleY = -89.0f;

                    t3Vector3f front;
                    front.x = t3Math::cosDeg(angleX) * t3Math::cosDeg(angleY);
                    front.y = t3Math::sinDeg(angleY);
                    front.z = t3Math::sinDeg(angleX) * t3Math::cosDeg(angleY);
                    direction = front.getNormalized();
                    up.set(0, 1, 0);
#ifdef TEST
                    mouseEvent->print(2);
#endif
                }
                else if(mouseEvent->type == s3MouseEvent::s3ButtonType::RIGHT)
                {
                    if (mouseEvent->offsetX != 0)
                        origin += mouseEvent->offsetX * keyboardSpeed * right;
                    if (mouseEvent->offsetY != 0)
                        origin += mouseEvent->offsetY * keyboardSpeed * direction;
                }
            }
            else if (data->sender == &s3CallbackManager::callBack.onMousePressed)
            {
                s3MouseEvent* mouseEvent = (s3MouseEvent*)data->userData;
#ifdef TEST
                mouseEvent->print(0);
#endif
            }
            else if (data->sender == &s3CallbackManager::callBack.onMouseReleased)
            {
                s3MouseEvent* mouseEvent = (s3MouseEvent*)data->userData;
#ifdef TEST
                mouseEvent->print(1);
#endif
            }
            else if (data->sender == &s3CallbackManager::callBack.onMouseScrolled)
            {
                s3MouseEvent* mouseEvent = (s3MouseEvent*)data->userData;
#ifdef TEST
                mouseEvent->print(3);
#endif
            }
        }

        // update view matrix
        camera->setCameraToWorld(origin, direction, up);
    }

    s3Camera* camera;
    float32 keyboardSpeed, mouseSpeed; 
    float32 angleX, angleY;
};

s3Camera::s3Camera(t3Vector3f origin, t3Vector3f direction, t3Vector3f up, float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    setCameraToWorld(origin, direction, up);
    setProjectionMatrix(aspectRatio, fovDeg, nearZ, farZ);

    // mouse + keyboard event
    cameraHandle = new s3CameraHandle(this);
    handleInit();

    mouseEventState = true;
    keyboardEventState = true;
}

void s3Camera::handleInit()
{
    s3CallbackManager::callBack.onKeyPressed += *cameraHandle;
    s3CallbackManager::callBack.onKeyReleased += *cameraHandle;
    s3CallbackManager::callBack.onMousePressed += *cameraHandle;
    s3CallbackManager::callBack.onMouseReleased += *cameraHandle;
    s3CallbackManager::callBack.onMouseScrolled += *cameraHandle;
    s3CallbackManager::callBack.onMouseMoved += *cameraHandle;
}

void s3Camera::handleDeinit()
{
    s3CallbackManager::callBack.onKeyPressed -= *cameraHandle;
    s3CallbackManager::callBack.onKeyReleased -= *cameraHandle;
    s3CallbackManager::callBack.onMousePressed -= *cameraHandle;
    s3CallbackManager::callBack.onMouseReleased -= *cameraHandle;
    s3CallbackManager::callBack.onMouseScrolled -= *cameraHandle;
    s3CallbackManager::callBack.onMouseMoved -= *cameraHandle;

    S3_SAFE_DELETE(cameraHandle);
}

s3Camera::~s3Camera()
{
    handleDeinit();
}

void s3Camera::setProjectionMatrix(float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    this->aspectRatio = aspectRatio;
    this->fovY = fovDeg;
    this->nearZ = nearZ;
    this->farZ = farZ;

    float halfFov = t3Math::Deg2Rad(fovY / 2.0f);
    float tanHalfFov = t3Math::tanRad(halfFov);
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

void s3Camera::setCameraToWorld(const t3Vector3f& origin, const t3Vector3f& dir, const t3Vector3f& cameraUp)
{
    //t3Vector3f direction = (lookAt - origin).getNormalized();
    t3Vector3f direction = dir.getNormalized();
    t3Vector3f right = cameraUp.getCrossed(direction).getNormalized();
    t3Vector3f up = direction.getCrossed(right).getNormalized();
    cameraToWorld.set(right.x, up.x, direction.x, origin.x,
                      right.y, up.y, direction.y, origin.y,
                      right.z, up.z, direction.z, origin.z,
                      0, 0, 0, 1);

    worldToCamera = cameraToWorld.getInverse();
}

void s3Camera::setCameraToWorld(const t3Matrix4x4 & cameraToWorld)
{
    this->cameraToWorld = cameraToWorld;
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

t3Vector3f s3Camera::getOrigin() const
{
    return t3Vector3f(cameraToWorld._mat[0][3], cameraToWorld._mat[1][3], cameraToWorld._mat[2][3]);
}

t3Vector3f s3Camera::getUpAxis() const
{
    return t3Vector3f(cameraToWorld._mat[0][1], cameraToWorld._mat[1][1], cameraToWorld._mat[2][1]);
}

t3Vector3f s3Camera::getDirectionAxis() const
{
    return t3Vector3f(cameraToWorld._mat[0][2], cameraToWorld._mat[1][2], cameraToWorld._mat[2][2]);
}

t3Vector3f s3Camera::getRightAxis() const
{
    return t3Vector3f(cameraToWorld._mat[0][0], cameraToWorld._mat[1][0], cameraToWorld._mat[2][0]);
}

void s3Camera::getViewAxis(t3Vector3f & origin, t3Vector3f & right, t3Vector3f& up, t3Vector3f& direction) const
{
    direction.set(cameraToWorld._mat[0][2], cameraToWorld._mat[1][2], cameraToWorld._mat[2][2]);
    origin.set(cameraToWorld._mat[0][3], cameraToWorld._mat[1][3], cameraToWorld._mat[2][3]);
    right.set(cameraToWorld._mat[0][0], cameraToWorld._mat[1][0], cameraToWorld._mat[2][0]);
    up.set(cameraToWorld._mat[0][1], cameraToWorld._mat[1][1], cameraToWorld._mat[2][1]);
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

float32 s3Camera::getFovY() const
{
    return fovY;
}

void s3Camera::setMouseSpeed(float mouseSpeed)
{
    cameraHandle->mouseSpeed = mouseSpeed;
}

void s3Camera::setKeyboardSpeed(float keyboardSpeed)
{
    cameraHandle->keyboardSpeed = keyboardSpeed;
}

void s3Camera::setMouseEventState(bool state)
{
    mouseEventState = state;
}

void s3Camera::setKeyboardEventState(bool state)
{
    keyboardEventState = state;
}

bool s3Camera::getMouseEventState() const
{
    return mouseEventState;
}

bool s3Camera::getKeyboardEventState() const
{
    return keyboardEventState;
}
