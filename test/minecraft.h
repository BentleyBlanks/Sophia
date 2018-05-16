#include <Sophia.h>
#include <core/log/s3Log.h>

class s3Minecraft : public s3CallbackHandle
{
public:
    void onHandle(const s3CallbackUserData* userData)
    {
        s3Log::debug("Minecraft!\n");
    }
};

int main()
{
    s3Minecraft mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    s3App app;
    if(!app.init())
        return 0;

    app.run();

    return 0;
}