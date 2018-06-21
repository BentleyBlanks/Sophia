#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

// State
ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;

// OM
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;

float32 width = 0, height = 0;
s3Camera* camera = nullptr;

int imageIndex = 0;
s3ImageDecoder image;

s3Shader skyShader;

struct cb
{
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;
    float padding;
    // -----------------------------------------------

    t3Matrix4x4 cameraToWorld;
};
cb cbCPU;
ID3D11Buffer* cbGPU;

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
        // IA
        deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
        deviceContext->IASetInputLayout(NULL);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // vs
        deviceContext->VSSetShader(skyShader.getVertexShader(), nullptr, 0);

        // ps
        ID3D11ShaderResourceView* srv = image.getShaderResouceView();
        ID3D11SamplerState* state = image.getSamplerState();

        deviceContext->PSSetShader(skyShader.getPixelShader(), nullptr, 0);
        deviceContext->PSSetSamplers(0, 1, &state);
        deviceContext->PSSetShaderResources(0, 1, &srv);

        cbCPU.cameraToWorld = camera->getCameraToWorld();
        deviceContext->UpdateSubresource(cbGPU, 0, nullptr, &cbCPU, 0, 0);
        deviceContext->PSSetConstantBuffers(0, 1, &cbGPU);

        deviceContext->RSSetState(rasterizerState);

        // OM
        deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
        deviceContext->OMSetDepthStencilState(depthStencilState, 1);

        deviceContext->Draw(3, 0);

        // gui
        guiRender();
    }
};

void createStates()
{
    bool MSAAEnabled = s3Renderer::get().getMSAAEnabled();
    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = FALSE;

    HRESULT hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create DepthStencil State\n");
        return;
    }

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.AntialiasedLineEnable = MSAAEnabled;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = MSAAEnabled;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state object.
    hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Rasterrizer State\n");
        return;
    }
}

void createShaders()
{
    skyShader.load(device, L"../Sophia/shaders/common/skyVS.hlsl", L"../Sophia/shaders/common/skyPS.hlsl");
}

void createConstantBuffer()
{
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cameraDirection + cameraOrigin
    constantBufferDesc.ByteWidth = sizeof(cb);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, &cbGPU);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create constant buffer[frame]\n");
        return;
    }

    // Update constant buffer's default value
    cbCPU.canvasDistance = 1;
    cbCPU.tanHalfFovY = t3Math::tanDeg(camera->getFovY() / 2.0f);
    cbCPU.tanHalfFovX = cbCPU.tanHalfFovY * camera->getAspectRatio();
}

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.2f));

    width = app.getWindow()->getWindowSize().x;
    height = app.getWindow()->getWindowSize().y;

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    camera = new s3Camera(t3Vector3f(0, 0, -1), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        width / height, 65, 0.1f, 1000.0f);

    createShaders();
    createStates();
    createConstantBuffer();

    image.load(device, "../resources/irradianceDiffuseMap1.exr");

    s3Demo mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();
    return 0;
}