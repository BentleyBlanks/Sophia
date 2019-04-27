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
    constantBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;\
    constantBufferDesc.ByteWidth           = sizeof(cbClassName);\
    constantBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;\
    constantBufferDesc.MiscFlags           = 0;\
    constantBufferDesc.StructureByteStride = 0;\
    constantBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;\
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

int32 width, height;
s3Camera* camera = nullptr;
s3Shader ssao, gbuffer, blur, lighting;

s3Texture normal, position, albedo, random;
s3Model model;

// preview
s3Shader drawImage;
int imageIndex = 0;

void createTextures()
{
    normal.createRT(width, height);
    position.createRT(width, height); 
    albedo.createRT(width, height); 

    random;
}

void createCB()
{
    createConstantBuffer(device, s3VSCB, vsCBGPU);
    vsCBCPU.model.makeIdentityMatrix();
}

void createShader()
{
    gbuffer.load(L"../Sophia/shaders/ssao/geometryVS.hlsl", L"../Sophia/shaders/ssao/geometryPS.hlsl");
}

class s3Demo : public s3CallbackHandle
{
public:
    void guiRender()
    {
        ImGui::Begin("Who's Your Daddy?");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            const char* textureNames[] = { "Normal", "Position", "Albedo", "Random" };
            ImGui::Combo("Textures", &imageIndex, textureNames, IM_ARRAYSIZE(textureNames));

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* imageData)
    {
        D3D11_MAPPED_SUBRESOURCE ms;

        ID3D11RenderTargetView* renderTargetView   = s3Renderer::get().getRenderTargetView();
        ID3D11DepthStencilView* depthStencilView   = s3Renderer::get().getDepthStencilView();
        ID3D11DepthStencilState* depthStencilState = s3Renderer::get().getDepthStencilState();
        ID3D11RasterizerState* rasterizerState     = s3Renderer::get().getRasterizerState();

        // GBuffer
        {        
            // IA
            deviceContext->IASetInputLayout(gbuffer.getInputLayout());

            // vs
            deviceContext->VSSetShader(gbuffer.getVertexShader(), nullptr, 0);
            ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
            if (SUCCEEDED(deviceContext->Map(vsCBGPU, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
            {
                vsCBCPU.projection = camera->getProjectionMatrix();
                vsCBCPU.view       = camera->getWorldToCamera();
                vsCBCPU.normalM    = t3Matrix4x4::getTransposedOf((vsCBCPU.view * vsCBCPU.model).getInverse());

                memcpy(ms.pData, &vsCBCPU, sizeof(vsCBCPU));
                deviceContext->Unmap(vsCBGPU, 0);
            }
            deviceContext->VSSetConstantBuffers(0, 1, &vsCBGPU);

            // ps
            deviceContext->PSSetShader(gbuffer.getPixelShader(), nullptr, 0);

            deviceContext->RSSetState(rasterizerState);

            // OM Multiple RenderTarget
            ID3D11RenderTargetView* rts[4] = 
            {
                normal.getRenderTargetView(),
                position.getRenderTargetView(),
                albedo.getRenderTargetView(),
                random.getRenderTargetView()
            };
            deviceContext->OMSetRenderTargets(4, &rts[0], NULL);
            deviceContext->OMSetDepthStencilState(depthStencilState, 1);

            model.draw();
        }

        // Render to Texture Preview
        {
            // IA
            deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
            deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
            deviceContext->IASetInputLayout(NULL);
            deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // vs
            deviceContext->VSSetShader(drawImage.getVertexShader(), nullptr, 0);

            // ps
            ID3D11ShaderResourceView* srv = nullptr;
            ID3D11SamplerState* state = nullptr;
            s3Texture* tex = nullptr;
            switch (imageIndex)
            {
            case 0:
                tex = &normal;
                break;
            case 1:
                tex = &position;
                break;
            case 2:
                tex = &albedo;
                break;
            case 3:
                tex = &random;
                break;
            }

            srv = tex->getShaderResouceView();
            state = tex->getSamplerState();

            deviceContext->PSSetShader(drawImage.getPixelShader(), nullptr, 0);
            deviceContext->PSSetSamplers(0, 1, &state);
            deviceContext->PSSetShaderResources(0, 1, &srv);

            deviceContext->RSSetState(rasterizerState);

            // OM
            deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
            deviceContext->OMSetDepthStencilState(depthStencilState, 1);

            deviceContext->Draw(3, 0);
        }

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
    device        = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();

    s3Window* window = app.getWindow();

    width  = window->getWindowSize().x;
    height = window->getWindowSize().y;

    camera = new s3Camera(
        t3Vector3f(0, 0, -25), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        window->getWindowSize().x / window->getWindowSize().y, 45, 0.01f, 10000.0f);
    camera->setKeyboardSpeed(10.0f);

    model.load("../resources/models/sponza1/sponza.obj");

    createTextures();
    createShader();
    createCB();

    s3Demo mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();
    return 0;
}