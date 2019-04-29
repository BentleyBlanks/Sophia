#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

s3Renderer* renderer = nullptr;

ID3D11Device* device               = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

class s3Cloud : public s3CallbackHandle
{
public:
    void guiRender()
    {
        ImGui::Begin("Cloud");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData * imageData)
    {
        //s3Log::info("This is cloud handle system\n");

        guiRender();
    }
};

int main()
{
    s3App app;
    renderer = &s3Renderer::get();

    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.1f, 0.1f, 0.1f, 1.0f));

    s3Window* window = app.getWindow();
	float32 width  = window->getWindowSize().x;
	float32 height = window->getWindowSize().y;

    device        = renderer->getDevice();
    deviceContext = renderer->getDeviceContext();

    s3Cloud cloud;
    s3CallbackManager::callBack.onBeginRender += cloud;

    app.run();

    return 0;
}