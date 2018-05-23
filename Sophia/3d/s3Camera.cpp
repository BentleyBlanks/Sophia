#include <3d/s3Camera.h>
#include <core/s3Callback.h>
#include <core/s3Event.h>
#include <app/s3CallbackManager.h>
#include <core/s3MathHepler.h>

#define TEST

class s3Camera::s3CameraHandle : public s3CallbackHandle
{
public:
    s3CameraHandle(s3Camera* camera) : camera(camera), speed(0.08f) {}

    void onHandle(const s3CallbackUserData* data)
    {
        t3Vector3f origin, right, up, direction;
        camera->getViewAxis(origin, right, up, direction);

        if (data->sender == &s3CallbackManager::callBack.onKeyPressed)
        {
            s3KeyEvent* keyEvent = (s3KeyEvent*)data->userData;
            if (keyEvent->key == 'w')
                origin += speed * direction;
            else if (keyEvent->key == 'a')
                origin += speed * -right;
            else if (keyEvent->key == 's')
                origin += speed * -direction;
            else if (keyEvent->key == 'd')
                origin += speed * right;
            
            if(keyEvent->keyCode == s3KeyCode::Up)
                origin += speed * up;
            else if (keyEvent->keyCode == s3KeyCode::Down)
                origin += speed * -up;
            else if (keyEvent->keyCode == s3KeyCode::Left)
                origin += speed * -right;
            else if (keyEvent->keyCode == s3KeyCode::Right)
                origin += speed * right;
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

            // dragged(move + pressed)
            if (mouseEvent->type == s3MouseEvent::s3ButtonType::LEFT)
            {
                if (mouseEvent->offsetX != 0)
                {
                    t3Matrix4x4 a = makeRotationMatrix(mouseEvent->offsetX * 0.1f, up);
                    //a.print("a");

                    // rotate 2 axis
                    direction = direction * a;
                    right = right * a;
                }
                
                if (mouseEvent->offsetY != 0)
                {
                    t3Matrix4x4 a = makeRotationMatrix(mouseEvent->offsetY * 0.1f, right);

                    // rotate 2 axis
                    direction = direction * a;
                    up = up * a;
                }
#ifdef TEST
                mouseEvent->print(2);
#endif
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

        // update view matrix
        camera->setCameraToWorld(origin, direction + origin, up);
    }

    s3Camera* camera;
    float speed;
};

s3Camera::s3Camera(t3Vector3f origin, t3Vector3f lookAt, t3Vector3f up, float32 aspectRatio, float32 fovDeg, float32 nearZ, float32 farZ)
{
    setCameraToWorld(origin, lookAt, up);
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
    t3Vector3f up = direction.getCrossed(right).getNormalized();
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

float32 s3Camera::getFov() const
{
    return fov;
}
