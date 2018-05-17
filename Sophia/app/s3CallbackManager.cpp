#include <app/s3CallbackManager.h>

s3Callbacks s3CallbackManager::callBack;

// handle system's event
class s3SystemHandler : public s3CallbackHandle
{
public:
    void onHandle(const s3CallbackUserData* userData)
    {
        // do something
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
}

void s3CallbackDeinit()
{
    s3CallbackManager::callBack.onBeginRender -= systemHandler;
    s3CallbackManager::callBack.onEndRender -= systemHandler;
    s3CallbackManager::callBack.onEngineDeinit -= systemHandler;
    s3CallbackManager::callBack.onEngineInit -= systemHandler;
    s3CallbackManager::callBack.onUpdate -= systemHandler;
}
