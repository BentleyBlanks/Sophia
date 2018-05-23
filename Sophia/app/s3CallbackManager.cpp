#include <app/s3CallbackManager.h>
#include <core/log/s3Log.h>
#include <core/s3Event.h>

s3Callbacks s3CallbackManager::callBack;

// handle system's event
class s3SystemHandler : public s3CallbackHandle
{
public:
    void onHandle(const s3CallbackUserData* data)
    {
        if (data->sender == &s3CallbackManager::callBack.onKeyReleased)
        {
            s3KeyEvent* keyEvent = (s3KeyEvent*)data->userData;
            if (keyEvent->keyCode == s3KeyCode::EscapeKey)
                exit(0);
        }
    }
};

s3SystemHandler systemHandler;

void s3CallbackInit()
{
    s3CallbackManager::callBack.onBeginRender += systemHandler;
    s3CallbackManager::callBack.onEndRender += systemHandler;
    s3CallbackManager::callBack.onEngineDeinit += systemHandler;
    s3CallbackManager::callBack.onEngineInit += systemHandler;
    s3CallbackManager::callBack.onUpdate += systemHandler;

    s3CallbackManager::callBack.onMousePressed += systemHandler;
    s3CallbackManager::callBack.onMouseMoved += systemHandler;
    s3CallbackManager::callBack.onMouseReleased += systemHandler;
    s3CallbackManager::callBack.onMouseScrolled += systemHandler;

    s3CallbackManager::callBack.onKeyPressed += systemHandler;
    s3CallbackManager::callBack.onKeyReleased += systemHandler;
}

void s3CallbackDeinit()
{
    s3CallbackManager::callBack.onBeginRender -= systemHandler;
    s3CallbackManager::callBack.onEndRender -= systemHandler;
    s3CallbackManager::callBack.onEngineDeinit -= systemHandler;
    s3CallbackManager::callBack.onEngineInit -= systemHandler;
    s3CallbackManager::callBack.onUpdate -= systemHandler;

    s3CallbackManager::callBack.onMousePressed -= systemHandler;
    s3CallbackManager::callBack.onMouseMoved -= systemHandler;
    s3CallbackManager::callBack.onMouseReleased -= systemHandler;
    s3CallbackManager::callBack.onMouseScrolled -= systemHandler;

    s3CallbackManager::callBack.onKeyPressed -= systemHandler;
    s3CallbackManager::callBack.onKeyReleased -= systemHandler;
}
