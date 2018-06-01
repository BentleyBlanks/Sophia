#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

// fullscreen triangle
//struct vertex
//{
//    t3Vector3f position;
//};
//
//const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
//{
//    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(s3VertexPNT, position), D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};
//
//vertex fullscreenTriangle[3] = 
//{
//    { t3Vector3f(-1,  2, 0) },
//    { t3Vector3f( 2, -1, 0) },
//    { t3Vector3f(-1, -1, 0) }
//};
//
//uint32 indicies[3] = { 0, 1, 2 };
//
//ID3D11Buffer* vertexBuffer;
//ID3D11Buffer* indexBuffer;

struct s3SkyCB
{
    // For computing perspective rays
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;
    float padding1;
    // --------------------------16Byte--------------------------

    // atmos + earth's origin
    t3Vector3f sphereOrigin;
    float padding3;
    // --------------------------16Byte--------------------------

    t3Matrix4x4 cameraToWorld;
    // --------------------------64Byte--------------------------
};

s3SkyCB skyCB;
ID3D11Buffer* constantBuffer;

// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

// Shader
//ID3D11InputLayout* inputLayout = nullptr;
ID3D11VertexShader* vertexShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;

// State
ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;

// OM
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;

// Camera
float32 width = 0, height = 0;
t3Matrix4x4 projectionMatrix, worldToCamera;
s3Camera* camera = nullptr;

class s3Sky : public s3CallbackHandle
{
public:
    void guiRender()
    {
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::Begin("HHHH");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);


            if (show_demo_window)
            {
                ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
                ImGui::ShowDemoWindow(&show_demo_window);
            }
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

        deviceContext->RSSetState(rasterizerState);

        // vs
        deviceContext->VSSetShader(vertexShader, nullptr, 0);

        // ps
        deviceContext->PSSetShader(pixelShader, nullptr, 0);

        // Update constant buffer
        skyCB.canvasDistance = 1;
        skyCB.tanHalfFovY = t3Math::tanDeg(camera->getFovY() / 2.0f);
        skyCB.tanHalfFovX = skyCB.tanHalfFovY * camera->getAspectRatio();
        skyCB.sphereOrigin = t3Vector3f(0, 0, 0);
        skyCB.cameraToWorld = camera->getCameraToWorld();

        deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &skyCB, 0, 0);
        deviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

        // OM
        deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
        deviceContext->OMSetDepthStencilState(depthStencilState, 1);

        deviceContext->Draw(3, 0);

        // gui
        guiRender();
    }
};

//void createVertexIndexBuffer()
//{
//    // resource loaded
//    // vertex buffer
//    D3D11_BUFFER_DESC vertexBufferDesc;
//    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
//    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vertexBufferDesc.ByteWidth = sizeof(vertex) * _countof(fullscreenTriangle);
//    vertexBufferDesc.CPUAccessFlags = 0;
//    vertexBufferDesc.MiscFlags = 0;
//    vertexBufferDesc.StructureByteStride = 0;
//    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//
//    // bind buffer's data
//    D3D11_SUBRESOURCE_DATA resourceData;
//    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
//    resourceData.pSysMem = fullscreenTriangle;
//
//    // create vertex buffer
//    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &resourceData, &vertexBuffer);
//    if(FAILED(hr))
//    {
//        s3Log::error("Failed to create vertex buffer.\n");
//        return;
//    }
//
//    // index buffer
//    D3D11_BUFFER_DESC indexBufferDesc;
//    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
//    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    indexBufferDesc.ByteWidth = sizeof(int32) * _countof(indicies);
//    indexBufferDesc.CPUAccessFlags = 0;
//    indexBufferDesc.MiscFlags = 0;
//    indexBufferDesc.StructureByteStride = 0;
//    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//
//    // bind buffer's data
//    resourceData.pSysMem = indicies;
//
//    // create index buffer
//    hr = device->CreateBuffer(&indexBufferDesc, &resourceData, &indexBuffer);
//    if(FAILED(hr))
//    {
//        s3Log::error("Failed to create index buffer.\n");
//        return;
//    }
//}

void createStates()
{
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
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
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
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cameraDirection + cameraOrigin
    constantBufferDesc.ByteWidth = sizeof(s3SkyCB);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create constant buffer[frame]\n");
        return;
    }
}

void createShaders()
{
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    std::string profile = s3GetLatestProfile(s3ShaderType::S3_VERTEX_SHADER);
    uint32 flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif
    std::wstring vsFilePath(L"../Sophia/shaders/skyVert.hlsl");
    std::wstring psFilePath(L"../Sophia/shaders/skyPixel.hlsl");

    // ---------------------------------vertex shader---------------------------------
    HRESULT hr = D3DCompileFromFile(
        vsFilePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        profile.c_str(),
        flags,
        0,
        &shaderBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            s3Log::error("Vertex Shader Compile Failed: %s", (char*)errorBlob->GetBufferPointer());
            S3_SAFE_RELEASE(shaderBlob);
            S3_SAFE_RELEASE(errorBlob);
            return;
        }
    }

    device->CreateVertexShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader);

    //hr = device->CreateInputLayout(
    //    vertexDesc,
    //    _countof(vertexDesc),
    //    shaderBlob->GetBufferPointer(),
    //    shaderBlob->GetBufferSize(),
    //    &inputLayout);

    if (FAILED(hr))
    {
        s3Log::error("Input Layout Create Failed\n");
        return;
    }

    // ---------------------------------pixel shader---------------------------------
    profile = s3GetLatestProfile(s3ShaderType::S3_PIXEL_SHADER);
    hr = D3DCompileFromFile(
        psFilePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        profile.c_str(),
        flags,
        0,
        &shaderBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            s3Log::error("Pixel Shader Compile Failed: %s", (char*)errorBlob->GetBufferPointer());
            S3_SAFE_RELEASE(shaderBlob);
            S3_SAFE_RELEASE(errorBlob);
            return;
        }
    }

    device->CreatePixelShader(shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        &pixelShader);

    S3_SAFE_RELEASE(shaderBlob);
    S3_SAFE_RELEASE(errorBlob);
}

void destroy()
{
    S3_SAFE_RELEASE(constantBuffer);
    //S3_SAFE_RELEASE(constantBuffers[APPLIATION]);
    //S3_SAFE_RELEASE(constantBuffers[FRAME]);
    //S3_SAFE_RELEASE(constantBuffers[OBJECT]);

    //S3_SAFE_RELEASE(indexBuffer);
    //S3_SAFE_RELEASE(vertexBuffer);
    //S3_SAFE_RELEASE(inputLayout);
    S3_SAFE_RELEASE(vertexShader);
    S3_SAFE_RELEASE(pixelShader);
}

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1280, 720), t3Vector2f(100, 100)))
        return 0;

    s3Window* window = app.getWindow();
    width = window->getWindowSize().x;
    height = window->getWindowSize().y;

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    camera = new s3Camera(t3Vector3f(0, 0, -15000.0f), t3Vector3f(0, 0, 0), t3Vector3f(0, 1, 0),
        width / height, 65, 0.1f, 6e5f);
    //camera = new s3Camera(t3Vector3f(0, 6360 + 20, -100), t3Vector3f(0, 6360 + 20, 0), t3Vector3f(0, 1, 0),
    //    width / height, 65, 0.001f, 10000.0f);

    //createVertexIndexBuffer();
    createStates();
    createConstantBuffers();
    createShaders();

    s3Sky mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();

    destroy(); 
    return 0;
}
