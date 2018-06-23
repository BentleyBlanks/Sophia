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

s3Renderer* renderer = nullptr;

// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

// Shaders
int32 shaderIndex = 0;
s3Shader* pbrShader = nullptr, *pbrIBLShader = nullptr, *skyShader = nullptr;
s3ImageDecoder albedoMap, normalMap, metallicMap, roughnessMap, aoMap;
s3ImageDecoder irradianceMap, prefilterMap, brdfMap;
s3ImageDecoder skybox;

struct s3PbrVSCB
{
    t3Matrix4x4 projection, view, model;
};
s3PbrVSCB pbrVSCBCPU;
ID3D11Buffer* pbrVSCBGPU;

struct s3SkyboxCB
{
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;
    float padding;
    // -----------------------------------------------

    t3Matrix4x4 cameraToWorld;
};
s3SkyboxCB skyboxCBCPU;
ID3D11Buffer* skyboxCBGPU;

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

    skyShader = new s3Shader();
    skyShader->load(device, L"../Sophia/shaders/common/skyVS.hlsl", L"../Sophia/shaders/common/skyPS.hlsl");
}

void createConstantBuffers()
{
    createConstantBuffer(device, s3PbrVSCB, pbrVSCBGPU);

    createConstantBuffer(device, s3PbrPSCB, pbrPSCBGPU);
    {
        pbrPSCBCPU.albedo = t3Vector3f(1.0f, 1.0f, 1.0f);
        pbrPSCBCPU.ao = 1.0f;

        for (int32 i = 0; i < 4; i++)
        {
            pbrPSCBCPU.lightPositions[i] = toVec4(pointLights[i].getPosition());
            pbrPSCBCPU.lightColors[i] = toVec4(pointLights[i].getColor());
        }
    }

    createConstantBuffer(device, s3SkyboxCB, skyboxCBGPU);
    {
        skyboxCBCPU.canvasDistance = 1;
        skyboxCBCPU.tanHalfFovY = t3Math::tanDeg(camera->getFovY() / 2.0f);
        skyboxCBCPU.tanHalfFovX = skyboxCBCPU.tanHalfFovY * camera->getAspectRatio();
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

            const char* imageNames[] = { "IBL", "Lights" };
            ImGui::Combo("Shader Selection", &shaderIndex, imageNames, IM_ARRAYSIZE(imageNames));

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* imageData)
    {
        // skybox
        {                    
            // IA
            deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
            deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
            deviceContext->IASetInputLayout(NULL);
            deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // vs
            deviceContext->VSSetShader(skyShader->getVertexShader(), nullptr, 0);

            // ps
            ID3D11ShaderResourceView* srv = skybox.getShaderResouceView();
            ID3D11SamplerState* state = skybox.getSamplerState();
            deviceContext->PSSetShader(skyShader->getPixelShader(), nullptr, 0);
            deviceContext->PSSetSamplers(0, 1, &state);
            deviceContext->PSSetShaderResources(0, 1, &srv);

            skyboxCBCPU.cameraToWorld = camera->getCameraToWorld();
            deviceContext->UpdateSubresource(skyboxCBGPU, 0, nullptr, &skyboxCBCPU, 0, 0);
            deviceContext->PSSetConstantBuffers(0, 1, &skyboxCBGPU);

            deviceContext->RSSetState(renderer->getRasterizerState());

            // OM
            deviceContext->OMSetRenderTargets(1, &renderer->getRenderTargetView(), renderer->getDepthStencilView());
            deviceContext->OMSetDepthStencilState(renderer->getDepthStencilState(), 1);

            deviceContext->Draw(3, 0);
        }

        // pbr 
        {
            s3Shader* shader = nullptr;
            switch (shaderIndex)
            {
            case 0:
                shader = pbrIBLShader;
                break;
            case 1:
                shader = pbrShader;
                break;
            }

            for (int32 i = 0; i < sphereRows; i++)
            {
                for (int32 j = 0; j < sphereColumns; j++)
                {
                    // IA
                    deviceContext->IASetInputLayout(shader->getInputLayout());

                    // VS
                    deviceContext->VSSetShader(shader->getVertexShader(), nullptr, 0);

                    pbrVSCBCPU.projection = camera->getProjectionMatrix();
                    pbrVSCBCPU.view = camera->getWorldToCamera();
                    pbrVSCBCPU.model = spheres[i * sphereColumns + j]->getObjectToWorld();
                    deviceContext->UpdateSubresource(pbrVSCBGPU, 0, nullptr, &pbrVSCBCPU, 0, 0);
                    deviceContext->VSSetConstantBuffers(0, 1, &pbrVSCBGPU);

                    // PS
                    deviceContext->PSSetShader(shader->getPixelShader(), nullptr, 0);

                    // PS Constant Buffer
                    pbrPSCBCPU.cameraPosition = camera->getOrigin();
                    pbrPSCBCPU.roughness = t3Math::clamp((float32)(sphereColumns - j) / sphereColumns, 0.05f, 1.0f);
                    pbrPSCBCPU.metallic = (float32)(sphereRows - i) / sphereRows;
                    deviceContext->UpdateSubresource(pbrPSCBGPU, 0, nullptr, &pbrPSCBCPU, 0, 0);
                    deviceContext->PSSetConstantBuffers(0, 1, &pbrPSCBGPU);

                    // PS Bind Textures
                    ID3D11SamplerState* sampler = albedoMap.getSamplerState();
                    deviceContext->PSSetSamplers(0, 1, &sampler);

                    // textures
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

                    // lut
                    ID3D11ShaderResourceView* srv6 = irradianceMap.getShaderResouceView();
                    ID3D11ShaderResourceView* srv7 = prefilterMap.getShaderResouceView();
                    ID3D11ShaderResourceView* srv8 = brdfMap.getShaderResouceView();
                    deviceContext->PSSetShaderResources(5, 1, &srv6);
                    deviceContext->PSSetShaderResources(6, 1, &srv7);
                    deviceContext->PSSetShaderResources(7, 1, &srv8);

                    deviceContext->RSSetState(renderer->getRasterizerState());

                    // OM
                    deviceContext->OMSetRenderTargets(1, &renderer->getRenderTargetView(), renderer->getDepthStencilView());
                    deviceContext->OMSetDepthStencilState(renderer->getDepthStencilState(), 1);

                    spheres[i * sphereColumns + j]->draw(deviceContext);
                }
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

    renderer = &s3Renderer::get();
    device = renderer->getDevice();
    deviceContext = renderer->getDeviceContext();

    camera = new s3Camera(t3Vector3f(0, 0, -25), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        width / height, 45, 0.01f, 1000.0f);

    createShaders();
    createConstantBuffers();

    // init spheres
    float32 spacing = 2.5;
    for (int32 i = 0; i < sphereRows; i++)
    {
        for (int32 j = 0; j < sphereColumns; j++)
        {
            spheres[i * sphereColumns + j] = s3Mesh::createSphere(renderer->getDeviceContext(), 1.0f, 64);
            spheres[i * sphereColumns + j]->setObjectToWorld(t3Matrix4x4(
                1, 0, 0, (i - (sphereRows / 2.0f)) * spacing,
                0, 1, 0, (j - (sphereColumns / 2.0f)) * spacing,
                0, 0, 1, 0,
                0, 0, 0, 1));
        }
    }

    // skybox
    skybox.load(device, "../resources/newport_loft.hdr");

    // pbr textures
    albedoMap.load(device, "../resources/textures/pbr/rusted_iron/albedo.png");
    normalMap.load(device, "../resources/textures/pbr/rusted_iron/normal.png");
    metallicMap.load(device, "../resources/textures/pbr/rusted_iron/metallic.png");
    roughnessMap.load(device, "../resources/textures/pbr/rusted_iron/roughness.png");
    aoMap.load(device, "../resources/textures/pbr/rusted_iron/ao.png");

    // precomputed lut
    irradianceMap.load(device, "../resources/lut/irradianceDiffuseMap.exr");
    brdfMap.load(device, "../resources/lut/brdfMap.exr");
    std::vector<std::string> specularNames;
    for (int32 i = 0; i < 8; i++)
    {
        std::string name = "../resources/lut/specular/";
        name += s3ToString(i);
        name += ".exr";
        specularNames.push_back(name);
    }
    prefilterMap.load(device, specularNames);

    s3Pbr mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();

    return 0;
}