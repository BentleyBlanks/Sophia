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
    constantBufferDesc.CPUAccessFlags = 0;\
    constantBufferDesc.MiscFlags = 0;\
    constantBufferDesc.StructureByteStride = 0;\
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;\
    HRESULT hr = d->CreateBuffer(&constantBufferDesc, nullptr, &cb);\
    if (FAILED(hr)){\
        s3Log::error("Failed to create constant buffer\n");\
        return;\
    }\
}

t3Vector4f toVec4(t3Vector3f v)
{
    return t3Vector4f(v.x, v.y, v.z, 1.0f);
}

const int32 sphereRows = 7, sphereColumns = 7;
s3Mesh* spheres[sphereRows * sphereColumns];

s3Camera* camera = nullptr;
// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

// State
ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;

// OM
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;

// Shaders
s3Shader* pbrShader = nullptr, *pbrIBLShader = nullptr;
s3ImageDecoder albedoMap, normalMap, metallicMap, roughnessMap, aoMap;

struct s3PbrVSCB
{
    t3Matrix4x4 projection, view, model;
};
s3PbrVSCB pbrVSCBCPU;
ID3D11Buffer* pbrVSCBGPU;

struct s3PbrPSCB
{
    // material parameters
    t3Vector3f albedo;
    float32 metallic;
    // -----------------------------

    float32 roughness;
    float32 ao;
    int32 useTexture;
    float32 padding0;
    // -----------------------------

    // lights
    t3Vector4f lightPositions[4];
    t3Vector4f lightColors[4];
    // -----------------------------

    t3Vector3f cameraPosition;
    float32 padding1;
    // -----------------------------
};
s3PbrPSCB pbrPSCBCPU;
ID3D11Buffer* pbrPSCBGPU;

s3PointLight pointLights[4] = {
    s3PointLight(t3Vector3f(-10.0f,  10.0f, -10.0f), t3Vector3f(300.0f, 300.0f, 300.0f)),
    s3PointLight(t3Vector3f(10.0f,  10.0f, -10.0f), t3Vector3f(300.0f, 300.0f, 300.0f)),
    s3PointLight(t3Vector3f(-10.0f, -10.0f, -10.0f), t3Vector3f(300.0f, 300.0f, 300.0f)),
    s3PointLight(t3Vector3f(10.0f, -10.0f, -10.0f), t3Vector3f(300.0f, 300.0f, 300.0f)),
};

void createShaders()
{
    pbrShader = new s3Shader();
    pbrShader->load(device, L"../Sophia/shaders/pbrLighting/pbrLightingVS.hlsl", L"../Sophia/shaders/pbrLighting/pbrLightingPS.hlsl");


    pbrIBLShader = new s3Shader();
    pbrIBLShader->load(device, L"../Sophia/shaders/pbrLighting/pbrIBLVS.hlsl", L"../Sophia/shaders/pbrLighting/pbrIBLPS.hlsl");
}

void createStates()
{
    bool MSAAEnabled = s3Renderer::get().getMSAAEnabled();
    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilStateDesc.DepthEnable = MSAAEnabled;
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

void createConstantBuffers()
{
    createConstantBuffer(device, s3PbrVSCB, pbrVSCBGPU);

    createConstantBuffer(device, s3PbrPSCB, pbrPSCBGPU);
    {
        pbrPSCBCPU.albedo = t3Vector3f(0.5f, 0.0f, 0.0f);
        pbrPSCBCPU.ao = 1.0f;

        for (int32 i = 0; i < 4; i++)
        {
            pbrPSCBCPU.lightPositions[i] = toVec4(pointLights[i].getPosition());
            pbrPSCBCPU.lightColors[i] = toVec4(pointLights[i].getColor());
        }
    }
}

class s3Pbr : public s3CallbackHandle
{
public:
    void guiRender()
    {
        ImGui::Begin("PBR");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            static bool mouseEventEnable = true;
            if (ImGui::Checkbox("Mouse Event Enabled", &mouseEventEnable))
                camera->setMouseEventState(mouseEventEnable);

            static bool useTexture = true;
            if (ImGui::Checkbox("Use Texture", &useTexture))
                pbrPSCBCPU.useTexture = (int32) useTexture;

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* userData)
    {
        for (int32 i = 0; i < sphereRows; i++)
        {
            for (int32 j = 0; j < sphereColumns; j++)
            {
                // IA
                deviceContext->IASetInputLayout(pbrShader->getInputLayout());

                // VS
                deviceContext->VSSetShader(pbrShader->getVertexShader(), nullptr, 0);

                pbrVSCBCPU.projection = camera->getProjectionMatrix();
                pbrVSCBCPU.view = camera->getWorldToCamera();
                pbrVSCBCPU.model = spheres[i * sphereColumns + j]->getObjectToWorld();
                deviceContext->UpdateSubresource(pbrVSCBGPU, 0, nullptr, &pbrVSCBCPU, 0, 0);
                deviceContext->VSSetConstantBuffers(0, 1, &pbrVSCBGPU);

                // PS
                deviceContext->PSSetShader(pbrShader->getPixelShader(), nullptr, 0);

                // PS Constant Buffer
                pbrPSCBCPU.cameraPosition = camera->getOrigin();
                pbrPSCBCPU.roughness = t3Math::clamp((float32)j / sphereColumns, 0.05f, 1.0f);
                pbrPSCBCPU.metallic = (float32)i / sphereRows;
                deviceContext->UpdateSubresource(pbrPSCBGPU, 0, nullptr, &pbrPSCBCPU, 0, 0);
                deviceContext->PSSetConstantBuffers(0, 1, &pbrPSCBGPU);

                // PS Bind Textures
                ID3D11SamplerState* sampler = albedoMap.getSamplerState();
                deviceContext->PSSetSamplers(0, 1, &sampler);

                ID3D11ShaderResourceView* srv1 = albedoMap.getShaderResouceView();
                ID3D11ShaderResourceView* srv2 = normalMap.getShaderResouceView();
                ID3D11ShaderResourceView* srv3 = metallicMap.getShaderResouceView();
                ID3D11ShaderResourceView* srv4 = roughnessMap.getShaderResouceView();
                ID3D11ShaderResourceView* srv5 = aoMap.getShaderResouceView();
                deviceContext->PSSetShaderResources(0, 1, &srv1);
                deviceContext->PSSetShaderResources(1, 1, &srv2);
                deviceContext->PSSetShaderResources(2, 1, &srv3);
                deviceContext->PSSetShaderResources(3, 1, &srv4);
                deviceContext->PSSetShaderResources(4, 1, &srv5);

                deviceContext->RSSetState(rasterizerState);

                // OM
                deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
                deviceContext->OMSetDepthStencilState(depthStencilState, 1);

                spheres[i * sphereColumns + j]->draw(deviceContext);
            }
        }

        guiRender();
    }
};

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.1f, 0.1f, 0.1f, 1.0f));

    float32 width = 0, height = 0;

    s3Window* window = app.getWindow();
    width = window->getWindowSize().x;
    height = window->getWindowSize().y;

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    camera = new s3Camera(t3Vector3f(0, 0, -25), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        width / height, 45, 0.01f, 1000.0f);

    createShaders();
    createStates();
    createConstantBuffers();

    // init spheres
    float32 spacing = 2.5;
    for (int32 i = 0; i < sphereRows; i++)
    {
        for (int32 j = 0; j < sphereColumns; j++)
        {
            spheres[i * sphereColumns + j] = s3Mesh::createSphere(renderer.getDeviceContext(), 1.0f, 64);
            spheres[i * sphereColumns + j]->setObjectToWorld(t3Matrix4x4(
                1, 0, 0, (i - (sphereRows / 2.0f)) * spacing,
                0, 1, 0, (j - (sphereColumns / 2.0f)) * spacing,
                0, 0, 1, 0,
                0, 0, 0, 1));
        }
    }

    albedoMap.load(device, "../resources/textures/pbr/rusted_iron/albedo.png");
    normalMap.load(device, "../resources/textures/pbr/rusted_iron/normal.png");
    metallicMap.load(device, "../resources/textures/pbr/rusted_iron/metallic.png");
    roughnessMap.load(device, "../resources/textures/pbr/rusted_iron/roughness.png");
    aoMap.load(device, "../resources/textures/pbr/rusted_iron/ao.png");

    s3Pbr mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();

    return 0;
}