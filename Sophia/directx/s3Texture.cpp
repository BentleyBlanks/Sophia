#include <directx/s3Texture.h>
#include <core/log/s3Log.h>
#include <app/s3Renderer.h>

// --------------------------------------Utils--------------------------------------
bool createSRV(ID3D11Device* device, ID3D11Texture2D*& texture2d, ID3D11ShaderResourceView*& textureSRV, DXGI_FORMAT format, uint32 mipLevels)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format                    = format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = mipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    HRESULT hr = device->CreateShaderResourceView(texture2d, &srvDesc, &textureSRV);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Shader Resource View\n");
        return false;
    }
    return true;
}

bool createSampler(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter         = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias     = 0.0f;
    samplerDesc.MaxAnisotropy  = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD         = -FLT_MAX;
    samplerDesc.MaxLOD         = FLT_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Sampler State\n");
        return false;
    }
    return true;
}

// miplevel = 1
bool createTexture(ID3D11Device* device, ID3D11Texture2D*& texture2d, uint8 bindFlags, DXGI_FORMAT format, int32 width, int32 height, void* dataPtr, int32 pitch)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize          = 1;
    textureDesc.BindFlags          = bindFlags;
    textureDesc.CPUAccessFlags     = 0;
    textureDesc.Format             = format;
    textureDesc.Width              = width;
    textureDesc.Height             = height;
    textureDesc.MipLevels          = 1;
    textureDesc.MiscFlags          = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;

    // Bind Data to Texture2D
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.SysMemSlicePitch = 0;
    initData.pSysMem          = dataPtr;
    initData.SysMemPitch      = pitch;

    HRESULT hr;
    if(dataPtr)
        hr = device->CreateTexture2D(&textureDesc, &initData, &texture2d);
    else
        hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2d);

    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        return false;
    }
    return true;
}

s3Texture::s3Texture() : 
    texture2d(nullptr), srv(nullptr), samplerState(nullptr), renderTargetView(nullptr),
    width(0), height(0)
{
}

s3Texture::~s3Texture()
{
    S3_SAFE_RELEASE(texture2d);
    S3_SAFE_RELEASE(srv);
    S3_SAFE_RELEASE(samplerState);
    S3_SAFE_RELEASE(renderTargetView);
}

bool s3Texture::load(int32 width, int32 height, std::vector<t3Vector4f> data)
{
    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    if (!createTexture(device, texture2d, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT, width, height, (void*)data.data(), sizeof(float32) * 4 * width))
        return false;

    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R32G32B32A32_FLOAT, 1))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::load(int32 width, int32 height, std::vector<unsigned char> data)
{
    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    if (!createTexture(device, texture2d, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM, width, height, (void*)data.data(), sizeof(uint8) * 4 * width))
        return false;

    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R8G8B8A8_UNORM, 1))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::load(int32 width, int32 height, float32* data)
{
    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    if (!createTexture(device, texture2d, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT, width, height, (void*)data, sizeof(float32) * 4 * width))
        return false;

    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R32G32B32A32_FLOAT, 1))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::load(int32 width, int32 height, std::vector<float32*> data)
{
    if (data.size() <= 0)
    {
        s3Log::warning("s3Texture load failed, null mipmap data\n");
        return false;
    }

    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize          = 1;
    textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags     = 0;
    textureDesc.MipLevels          = (uint32)data.size();
    textureDesc.MiscFlags          = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;

    textureDesc.Width              = width;
    textureDesc.Height             = height;
    textureDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;

    std::vector<D3D11_SUBRESOURCE_DATA> initialData(data.size());

    int32 w = width, h = height;
    for (int32 i = 0; i < data.size(); i++)
    {
        // Bind Data to Texture2D
        ZeroMemory(&initialData[i], sizeof(D3D11_SUBRESOURCE_DATA));
        initialData[i].SysMemSlicePitch = 0;
        initialData[i].pSysMem          = data[i];
        initialData[i].SysMemPitch      = sizeof(float32) * 4 * w;

        w = w / 2;
        h = h / 2;
    }

    HRESULT hr = device->CreateTexture2D(&textureDesc, initialData.data(), &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        return false;
    }    
    
    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R32G32B32A32_FLOAT, (uint32) data.size()))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::load(int32 width, int32 height, std::vector<std::vector<t3Vector4f>> data)
{
    if (data.size() <= 0)
    {
        s3Log::warning("s3Texture load failed, null mipmap data\n");
        return false;
    }

    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize          = 1;
    textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags     = 0;
    textureDesc.MipLevels          = (uint32)data.size();
    textureDesc.MiscFlags          = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;

    textureDesc.Width              = width;
    textureDesc.Height             = height;
    textureDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;

    std::vector<D3D11_SUBRESOURCE_DATA> initialData(data.size());

    int32 w = width, h = height;
    for (int32 i = 0; i < data.size(); i++)
    {
        // Bind Data to Texture2D
        ZeroMemory(&initialData[i], sizeof(D3D11_SUBRESOURCE_DATA));
        initialData[i].SysMemSlicePitch = 0;
        initialData[i].pSysMem = data[i].data();
        initialData[i].SysMemPitch = sizeof(float32) * 4 * w;

        w = w / 2;
        h = h / 2;
    }

    HRESULT hr = device->CreateTexture2D(&textureDesc, initialData.data(), &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        return false;
    }

    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R32G32B32A32_FLOAT, (uint32)data.size()))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::load(int32 width, int32 height, std::vector<std::vector<unsigned char>> data)
{
    if (data.size() <= 0)
    {
        s3Log::warning("s3Texture load failed, null mipmap data\n");
        return false;
    }

    ID3D11Device* device = s3Renderer::get().getDevice();

    this->width = width;
    this->width = height;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize          = 1;
    textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags     = 0;
    textureDesc.MipLevels          = (uint32)data.size();
    textureDesc.MiscFlags          = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;
    textureDesc.Width              = width;
    textureDesc.Height             = height;
    textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;

    std::vector<D3D11_SUBRESOURCE_DATA> initialData(data.size());

    int32 w = width, h = height;
    for (int32 i = 0; i < data.size(); i++)
    {
        // Bind Data to Texture2D
        ZeroMemory(&initialData[i], sizeof(D3D11_SUBRESOURCE_DATA));
        initialData[i].SysMemSlicePitch = 0;
        initialData[i].pSysMem = data[i].data();
        initialData[i].SysMemPitch = sizeof(uint8) * 4 * w;

        w = w / 2;
        h = h / 2;
    }

    HRESULT hr = device->CreateTexture2D(&textureDesc, initialData.data(), &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        return false;
    }

    // Create texture's relative shader resource view
    if (!createSRV(device, texture2d, srv, DXGI_FORMAT_R8G8B8A8_UNORM, (uint32)data.size()))
        return false;

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
        return false;

    return true;
}

bool s3Texture::createRT(int32 width, int32 height)
{
    ID3D11Device* device = s3Renderer::get().getDevice();

    if (!texture2d)
    {
        s3Log::warning("Texture2d already created, would be released first\n");
        S3_SAFE_RELEASE(texture2d);
    }

    if (!createTexture(device, texture2d, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT, width, height, nullptr, 0))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC desc;
    desc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;

    HRESULT hr = device->CreateRenderTargetView(texture2d, &desc, &renderTargetView);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Render Target\n");
        return false;
    }

    return true;
}

ID3D11Texture2D*& s3Texture::getTexture2D()
{
    return texture2d;
}

ID3D11ShaderResourceView*& s3Texture::getShaderResouceView()
{
    return srv;
}

ID3D11SamplerState*& s3Texture::getSamplerState()
{
    return samplerState;
}

ID3D11RenderTargetView*& s3Texture::getRenderTargetView()
{
    return renderTargetView;
}
