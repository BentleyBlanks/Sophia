#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>

enum constanBuffer
{
    APPLIATION,
    FRAME,
    OBJECT,
    NUM
};

struct vertex
{
    t3Vector3f position;
    t3Vector3f color;
};

vertex vertices[8] =
{
    {t3Vector3f(-1.0f, -1.0f, -1.0f), t3Vector3f(0.0f, 0.0f, 0.0f)}, // 0
    {t3Vector3f(-1.0f,  1.0f, -1.0f), t3Vector3f(0.0f, 1.0f, 0.0f)}, // 1
    {t3Vector3f(1.0f,  1.0f, -1.0f), t3Vector3f(1.0f, 1.0f, 0.0f)}, // 2
    {t3Vector3f(1.0f, -1.0f, -1.0f), t3Vector3f(1.0f, 0.0f, 0.0f)}, // 3
    {t3Vector3f(-1.0f, -1.0f,  1.0f), t3Vector3f(0.0f, 0.0f, 1.0f)}, // 4
    {t3Vector3f(-1.0f,  1.0f,  1.0f), t3Vector3f(0.0f, 1.0f, 1.0f)}, // 5
    {t3Vector3f(1.0f,  1.0f,  1.0f), t3Vector3f(1.0f, 1.0f, 1.0f)}, // 6
    {t3Vector3f(1.0f, -1.0f,  1.0f), t3Vector3f(1.0f, 0.0f, 1.0f)}  // 7
};

uint32 indicies[36] =
{
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};

ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr; 

ID3D11Buffer *indexBuffer = nullptr, *vertexBuffer = nullptr;
ID3D11Buffer *constantBuffers[NUM];

ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

ID3D11VertexShader* vertexShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr; 
ID3D11InputLayout* inputLayout = nullptr; 

ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;

float32 width = 0, height = 0;
t3Matrix4x4 projectionMatrix, worldToCamera, objectToWorld, worldToObject;
s3Camera* camera = nullptr;

class s3Minecraft : public s3CallbackHandle
{
public:
    void onHandle(const s3CallbackUserData* userData)
    {
        // update
        worldToCamera = camera->getWorldToCamera();
        deviceContext->UpdateSubresource(constantBuffers[FRAME], 0, nullptr, &worldToCamera, 0, 0);

        worldToObject.makeIdentityMatrix();
        objectToWorld = worldToObject.getInverse();
        deviceContext->UpdateSubresource(constantBuffers[OBJECT], 0, nullptr, &objectToWorld, 0, 0);

        // render
        uint32 vertexStride = sizeof(vertex);
        uint32 offset = 0;

        deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);
        deviceContext->IASetInputLayout(inputLayout);
        deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        deviceContext->VSSetShader(vertexShader, nullptr, 0);
        deviceContext->VSSetConstantBuffers(0, 3, constantBuffers);

        deviceContext->RSSetState(rasterizerState);

        deviceContext->PSSetShader(pixelShader, nullptr, 0);

        deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
        deviceContext->OMSetDepthStencilState(depthStencilState, 1);

        deviceContext->DrawIndexed(_countof(indicies), 0, 0);
    }
};

void createVertexIndexBuffer()
{
    // resource loaded
    // vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(vertex) * _countof(vertices);
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // bind buffer's data
    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    resourceData.pSysMem = vertices;

    // create vertex buffer
    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &resourceData, &vertexBuffer);
    if(FAILED(hr))
    {
        s3Log::error("Failed to create vertex buffer.\n");
        return;
    }

    // index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = sizeof(int32) * _countof(indicies);
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    // bind buffer's data
    resourceData.pSysMem = indicies;

    // create index buffer
    hr = device->CreateBuffer(&indexBufferDesc, &resourceData, &indexBuffer);
    if(FAILED(hr))
    {
        s3Log::error("Failed to create index buffer.\n");
        return;
    }
}

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
    if(FAILED(hr))
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
    if(FAILED(hr))
    {
        s3Log::error("Failed to create Rasterrizer State\n");
        return;
    }
}

void createConstantBuffers()
{
    // one cb contains one matrix
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(t3Matrix4x4);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffers[APPLIATION]);
    if(FAILED(hr))
    {
        s3Log::error("Failed to create constant buffer[application]\n");
        return;
    }

    hr = device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffers[FRAME]);
    if(FAILED(hr))
    {
        s3Log::error("Failed to create constant buffer[frame]\n");
        return;
    }

    hr = device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffers[OBJECT]);
    if(FAILED(hr))
    {
        s3Log::error("Failed to create constant buffer[object]\n");
        return;
    }

    // bind the perspective matrix
    projectionMatrix = camera->getProjectionMatrix();

    deviceContext->UpdateSubresource(constantBuffers[APPLIATION], 0, nullptr, &projectionMatrix, 0, 0);
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
    std::wstring vsFilePath(L"../Sophia/shaders/vertex.hlsl");
    std::wstring psFilePath(L"../Sophia/shaders/pixel.hlsl");

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

    if(FAILED(hr))
    {
        if(errorBlob)
        {
            s3Log::error("Vertex Shader Compile Failed: %s", (char*) errorBlob->GetBufferPointer());
            S3_SAFE_RELEASE(shaderBlob);
            S3_SAFE_RELEASE(errorBlob);
            return;
        }
    }

    device->CreateVertexShader(shaderBlob->GetBufferPointer(),
                               shaderBlob->GetBufferSize(),
                               nullptr,
                               &vertexShader);

    // input layout for vertex shader
    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = device->CreateInputLayout(
        vertexLayoutDesc,
        _countof(vertexLayoutDesc),
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        &inputLayout);

    if(FAILED(hr))
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

    if(FAILED(hr))
    {
        if(errorBlob)
        {
            s3Log::error("Pixel Shader Compile Failed: %s", (char*) errorBlob->GetBufferPointer());
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
    S3_SAFE_RELEASE(constantBuffers[APPLIATION]);
    S3_SAFE_RELEASE(constantBuffers[FRAME]);
    S3_SAFE_RELEASE(constantBuffers[OBJECT]);
    S3_SAFE_RELEASE(indexBuffer);
    S3_SAFE_RELEASE(vertexBuffer);
    S3_SAFE_RELEASE(inputLayout);
    S3_SAFE_RELEASE(vertexShader);
    S3_SAFE_RELEASE(pixelShader);
}

int main()
{
    s3Minecraft mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    s3App app;
    if(!app.init(t3Vector2f(1280, 720), t3Vector2f(100, 100)))
        return 0;

    s3Window* window = app.getWindow();
    width = window->getWindowSize().x;
    height = window->getWindowSize().y;

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    camera = new s3Camera(t3Vector3f(0, 0, -10), t3Vector3f(0, 0, 0), t3Vector3f(0, 1, 0),
                                 width / height, 45, 0.1f, 100.0f);


    createVertexIndexBuffer();
    createStates();
    createConstantBuffers();
    createShaders();

    app.run();

    destroy();
    return 0;
}
