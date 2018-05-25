#include <core/s3Image.h>
#include <lodepng.h>
#include <core/log/s3Log.h>

s3Image::s3Image()
    :texture2d(nullptr), textureSRV(nullptr), samplerState(nullptr),
    width(-1), height(-1), loaded(false)
{
}

s3Image::~s3Image()
{
}

bool s3Image::load(ID3D11Device* device, const std::string & filePath)
{
    std::vector<unsigned char> image;
    unsigned w, h;

    unsigned error = lodepng::decode(image, w, h, filePath.c_str());

    if (error)
    {
        s3Log::error("s3ImageDecoder load:%s failed, Error: %s", filePath.c_str(), lodepng_error_text(error));
        loaded = false;
        return false;
    }

    width = (int32) w;
    height = (int32) h;

    imageData.clear();
    for (int32 i = 0; i < height; i++)
    {
        for (int32 j = 0; j < width; j++)
        {
            //s3Log::print("r: %d, ", image[i * width + j + 0]);
            //s3Log::print("g: %d, ", image[i * width + j + 1]);
            //s3Log::print("b: %d, ", image[i * width + j + 2]);
            //s3Log::print("a: %d, ", image[i * width + j + 3]);

            imageData.push_back(t3Vector3f(image[(i * width + j) * 4 + 0] / 255.0f,
                                           image[(i * width + j) * 4 + 1] / 255.0f,
                                           image[(i * width + j) * 4 + 2] / 255.0f));
        }
    }

    // DX11 Texture2D
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.MiscFlags = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;

    // Bind Data to Texture2D
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = imageData.data();
    initData.SysMemPitch = sizeof(t3Vector3f) * width;
    initData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        loaded = false;
        return false;
    }

    // Create texture's relative shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(texture2d, &srvDesc, &textureSRV);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Shader Resource View\n");
        loaded = false;
        return false;
    }

    // Sampler State for texture sampling
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;

    hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Sampler State\n");
        loaded = false;
        return false;
    }

    loaded = true;
    return true;
}

int32 s3Image::getWidth()
{
    return width;
}

int32 s3Image::getHeight()
{
    return height;
}

ID3D11Texture2D * s3Image::getTexture2D()
{
    return texture2d;
}

ID3D11ShaderResourceView * s3Image::getShaderResouceView()
{
    return textureSRV;
}

ID3D11SamplerState * s3Image::getSamplerState()
{
    return samplerState;
}

std::vector<t3Vector3f>* s3Image::getImageData()
{
    return &imageData;
}

bool s3Image::isLoaded()
{
    return loaded;
}
