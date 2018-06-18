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

int imageIndex = 0;
s3ImageDecoder pngImage, exrImage, hdrImage;

s3Shader commonShader;

class s3Demo : public s3CallbackHandle
{
public:
    void guiRender()
    {
        ImGui::Begin("Who's Your Daddy?");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            const char* imageNames[] = { "PNG", "EXR", "HDR" };
            ImGui::Combo("Image Class", &imageIndex, imageNames, IM_ARRAYSIZE(imageNames));

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* userData)
    {
        // IA
        deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
        deviceContext->IASetInputLayout(NULL);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // vs
        deviceContext->VSSetShader(commonShader.getVertexShader(), nullptr, 0);

        // ps
        ID3D11ShaderResourceView* srv;
        ID3D11SamplerState* state;
        switch (imageIndex)
        {
        case 0:
            srv = pngImage.getShaderResouceView();
            state = pngImage.getSamplerState();
            break;
        case 1:
            srv = exrImage.getShaderResouceView();
            state = exrImage.getSamplerState();
            break;
        case 2:
            srv = hdrImage.getShaderResouceView();
            state = hdrImage.getSamplerState();
            break;
        }

        deviceContext->PSSetShader(commonShader.getPixelShader(), nullptr, 0);
        deviceContext->PSSetSamplers(0, 1, &state);
        deviceContext->PSSetShaderResources(0, 1, &srv);

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
    commonShader.load(device, L"../Sophia/shaders/common/drawImageVS.hlsl", L"../Sophia/shaders/common/drawImagePS.hlsl");
}

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.2f));

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    createShaders();
    createStates();

    pngImage.load(device, "../resources/03.png");
    exrImage.load(device, "../resources/skylightBlue.exr");
    hdrImage.load(device, "../resources/newport_loft.hdr");

    s3Demo mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();
    return 0;
}