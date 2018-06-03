#include <3d/s3Camera.h>
#include <core/s3Callback.h>
#include <core/s3Event.h>
#include <app/s3CallbackManager.h>
#include <core/s3MathHepler.h>

static float accumulativeAngleX = 0.0f, accumulativeAngleY = 0.0f;

class s3Camera::s3CameraHandle : public s3CallbackHandle
{
public:
    s3CameraHandle(s3Camera* camera) : camera(camera), keyboardSpeed(1e5f), mouseSpeed(1) {}

    void onHandle(const s3CallbackUserData* data)
    {
        t3Vector3f origin, right, up, direction;
        camera->getViewAxis(origin, right, up, direction);

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
            
            if(keyEvent->keyCode == s3KeyCode::Up)
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
        else if (data->sender == &s3CallbackManager::callBack.onMouseMoved)
        {
            s3MouseEvent* mouseEvent = (s3MouseEvent*)data->userData;

            //t3Vector3f d(0, 0, 1), u(0, 1, 0), r(1, 0, 0);
            //// dragged(move + pressed)
            //if (mouseEvent->type == s3MouseEvent::s3ButtonType::LEFT)
            //{
            //    if (mouseEvent->offsetX != 0)
            //    {
            //        accumulativeAngleX = fmodf(accumulativeAngleX + mouseEvent->offsetX * mouseSpeed, 360.0f);
            //        t3Matrix4x4 a = makeRotationMatrix(accumulativeAngleX, origin, up);

            //        // rotate 2 axis
            //        d = (d * a - origin).getNormalized();
            //        r = (r * a - origin).getNormalized();
            //        u = d.getCrossed(r).getNormalized();
            //    }
            //    
            //    if (mouseEvent->offsetY != 0)
            //    {
            //        accumulativeAngleY = fmodf(accumulativeAngleY + mouseEvent->offsetY * mouseSpeed, 360.0f);
            //        t3Matrix4x4 a = makeRotationMatrix(accumulativeAngleY, origin, right);

            //        // rotate 2 axis
            //        d = (d * a - origin).getNormalized();
            //        u = (u * a - origin).getNormalized();
            //        r = u.getCrossed(d).getNormalized();
            //    }

            //    direction = d;
            //    up = u;
            //    right = r;
#ifdef TEST
                mouseEvent->print(2);
#endif
            //}
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

        // update view matrix
        camera->setCameraToWorld(origin, direction, up);
    }

    s3Camera* camera;
    float keyboardSpeed, mouseSpeed;
};

s3Camera::s3Camera(t3Vector3f origin, t3Vector3f direction, t3Vector3f up, float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    setCameraToWorld(origin, direction, up);
    setProjectionMatrix(aspectRatio, fovDeg, nearZ, farZ);

    // mouse + keyboard event
    cameraHandle = new s3CameraHandle(this);
    handleInit();
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

void s3Camera::getViewAxis(t3Vector3f & origin, t3Vector3f & right, t3Vector3f& up, t3Vector3f& direction)
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
