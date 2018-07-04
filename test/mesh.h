#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

#define createConstantBuffer(d, cbClassName, cb)\
{\
    D3D11_BUFFER_DESC constantBufferDesc;\
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));\
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;\
    constantBufferDesc.ByteWidth = sizeof(cbClassName);\
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;\
    constantBufferDesc.MiscFlags = 0;\
    constantBufferDesc.StructureByteStride = 0;\
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;\
    HRESULT hr = d->CreateBuffer(&constantBufferDesc, nullptr, &cb);\
    if (FAILED(hr)){\
        s3Log::error("Failed to create constant buffer, hr: %d\n", hr);\
        return;\
    }\
}

struct s3VSCB
{
    t3Matrix4x4 projection, view, model, normalM;
};
s3VSCB vsCBCPU;
ID3D11Buffer* vsCBGPU;

// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

s3Camera* camera = nullptr;
s3ImageDecoder image;
s3Shader meshShader;
s3Mesh model;

void createCB()
{
    createConstantBuffer(device, s3VSCB, vsCBGPU);
    vsCBCPU.model.makeIdentityMatrix();
}

class s3Demo : public s3CallbackHandle
{
public:
    void guiRender()
    {
        ImGui::Begin("Who's Your Daddy?");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* imageData)
    {
        D3D11_MAPPED_SUBRESOURCE ms;

        // IA
        deviceContext->IASetInputLayout(meshShader.getInputLayout());

        // vs
        deviceContext->VSSetShader(meshShader.getVertexShader(), nullptr, 0);
        ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
        if (SUCCEEDED(deviceContext->Map(vsCBGPU, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            vsCBCPU.projection = camera->getProjectionMatrix();
            vsCBCPU.view = camera->getWorldToCamera();
            vsCBCPU.normalM = t3Matrix4x4::getTransposedOf((vsCBCPU.view * vsCBCPU.model).getInverse());

            memcpy(ms.pData, &vsCBCPU, sizeof(vsCBCPU));
            deviceContext->Unmap(vsCBGPU, 0);
        }
        deviceContext->VSSetConstantBuffers(0, 1, &vsCBGPU);

        // ps
        deviceContext->PSSetShader(meshShader.getPixelShader(), nullptr, 0);

        deviceContext->RSSetState(s3Renderer::get().getRasterizerState());

        // OM
        deviceContext->OMSetRenderTargets(1, &s3Renderer::get().getRenderTargetView(), s3Renderer::get().getDepthStencilView());
        deviceContext->OMSetDepthStencilState(s3Renderer::get().getDepthStencilState(), 1);

        model.draw();

        // gui
        guiRender();
    }
};

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.1f, 0.1f, 0.1f, 1.0f));

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();

    s3Window* window = app.getWindow();
    camera = new s3Camera(
        t3Vector3f(0, 0, -25), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        window->getWindowSize().x / window->getWindowSize().y, 45, 0.01f, 10000.0f);
    camera->setKeyboardSpeed(10.0f);

    meshShader.load(L"../Sophia/shaders/common/modelVS.hlsl", L"../Sophia/shaders/common/modelPS.hlsl");
    model.load("../resources/models/sponza1/sponza.obj");

    createCB();

    s3Demo mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();
    return 0;
}