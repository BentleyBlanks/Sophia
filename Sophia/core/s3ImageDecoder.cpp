#include <core/s3ImageDecoder.h>
#include <core/log/s3Log.h>
#include <app/s3Utils.h>

#include <lodepng.h>
#include <tinyexr.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// --------------------------------------Utils--------------------------------------
bool createSRV(DXGI_FORMAT format, uint32 mipLevels, ID3D11Device*& device, ID3D11Texture2D*& texture2d, ID3D11ShaderResourceView*& textureSRV)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
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
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Sampler State\n");
        return false;
    }
    return true;
}

// --------------------------------------s3ImageDecoder--------------------------------------
s3ImageDecoder::s3ImageDecoder()
    :texture2d(nullptr), textureSRV(nullptr), samplerState(nullptr),
    width(-1), height(-1), loaded(false), type(S3_IMAGE_ERROR), exrData(nullptr), hdrData(nullptr)
{
}

s3ImageDecoder::~s3ImageDecoder()
{
    //imageData.clear();
}

bool s3ImageDecoder::load(ID3D11Device* device, const std::string & filePath)
{
    s3Log::debug("Image:%s Begin loading...\n", filePath.c_str());

    // --------------------------------------Texture From File--------------------------------------
    DXGI_FORMAT format;
    type = s3GetImageType(filePath);

    if (type == S3_IMAGE_ERROR)
    {
        s3Log::error("s3ImageDecoder load failed, image postfix not suppoted\n");
        return false;
    }

    // For different type of image
    if (type == S3_IMAGE_PNG)
    {
        unsigned w, h;
        unsigned error = lodepng::decode(pngData, w, h, filePath.c_str());

        if (error)
        {
            s3Log::error("s3ImageDecoder load:%s failed, Error: %s\n", filePath.c_str(), lodepng_error_text(error));
            loaded = false;
            return false;
        }

        width = (int32)w;
        height = (int32)h;
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    else if (type == S3_IMAGE_EXR)
    {
        const char* error;
        if (LoadEXR(&exrData, &width, &height, filePath.c_str(), &error) != 0)
        {
            s3Log::error("s3ImageDecoder load:%s failed, Error: %s\n", error);
            exrData = NULL;
            return false;
        }
        format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else if (type == S3_IMAGE_HDR)
    {
        int numComponents;
        // desired 4 channels
        hdrData = stbi_loadf(filePath.c_str(), &width, &height, &numComponents, 4);

        if (!hdrData)
        {
            s3Log::error("s3ImageDecoder load:%s failed\n");
            return false;
        }
        format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    // --------------------------------------DX11 Texture2D--------------------------------------
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.Format = format;
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
    initData.SysMemSlicePitch = 0;

    if (type == S3_IMAGE_PNG)
    {
        initData.pSysMem = pngData.data();
        initData.SysMemPitch = sizeof(uint8) * 4 * width;
    }
    else if (type == S3_IMAGE_EXR)
    {
        initData.pSysMem = exrData;
        initData.SysMemPitch = sizeof(float32) * 4 * width;
    }
    else if (type == S3_IMAGE_HDR)
    {
        initData.pSysMem = hdrData;
        initData.SysMemPitch = sizeof(float32) * 4 * width;
    }

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
    srvDesc.Format = format;
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
    
    // Create texture's relative shader resource view
    if (!createSRV(format, 1, device, texture2d, textureSRV))
    {
        loaded = false;
        return false;
    }

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
    {
        loaded = false;
        return false;
    }

    s3Log::success("Image:%s loaded Successfully\n", filePath.c_str());
    loaded = true;

    return true;
}

bool s3ImageDecoder::load(ID3D11Device * device, int32 width, int32 height, const std::vector<t3Vector4f>& data)
{
    if (data.size() <= 0 || width <= 0 || height <= 0)
    {
        s3Log::warning("Image Data Null\n");
        return false;
    }

    this->width = width;
    this->height = height;

    // copy to local
    imageData.assign(data.begin(), data.end());

    // DX11 Texture2D
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
    initData.SysMemPitch = sizeof(t3Vector4f) * width;
    initData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        loaded = false;
        return false;
    }

    // Create texture's relative shader resource view
    if (!createSRV(DXGI_FORMAT_R32G32B32A32_FLOAT, 1, device, texture2d, textureSRV))
    {
        loaded = false;
        return false;
    }

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
    {
        loaded = false;
        return false;
    }

    s3Log::success("Texture2D created Successfully\n");
    loaded = true;
    return true;
}

bool s3ImageDecoder::load(ID3D11Device * device, const std::vector<std::string> mipFilesNames)
{
    if (mipFilesNames.size() <= 0)
    {
        s3Log::warning("s3ImageDecoder load failed, null mipFilesNames\n");
        loaded = false;
        return false;
    }

    // --------------------------------------DX11 Texture2D--------------------------------------
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.ArraySize = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MipLevels = (uint32) mipFilesNames.size();
    textureDesc.MiscFlags = 0;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;

    DXGI_FORMAT format;
    std::vector<D3D11_SUBRESOURCE_DATA> initialData(mipFilesNames.size());
    for (int32 i = 0; i < mipFilesNames.size(); i++)
    {
        std::string filePath = mipFilesNames[i];
        s3Log::debug("Image:%s Begin loading...\n", filePath.c_str());

        // texture ptr
        std::vector<unsigned char> tempPngData;
        float32 *tempExrData = nullptr, *tempHdrData = nullptr;

        // texture size
        int32 tempWidth = 0, tempHeight = 0;

        // --------------------------------------Texture From File--------------------------------------
        type = s3GetImageType(filePath);

        if (type == S3_IMAGE_ERROR)
        {
            s3Log::error("s3ImageDecoder load failed, image postfix not suppoted\n");
            loaded = false;
            return false;
        }

        // For different type of image
        if (type == S3_IMAGE_PNG)
        {
            unsigned w, h;
            unsigned error = lodepng::decode(tempPngData, w, h, filePath.c_str());

            if (error)
            {
                s3Log::error("s3ImageDecoder load:%s failed, Error: %s\n", filePath.c_str(), lodepng_error_text(error));
                loaded = false;
                return false;
            }

            tempWidth = (int32)w;
            tempHeight = (int32)h;
            format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
        else if (type == S3_IMAGE_EXR)
        {
            const char* error;
            if (LoadEXR(&tempExrData, &tempWidth, &tempHeight, filePath.c_str(), &error) != 0)
            {
                s3Log::error("s3ImageDecoder load:%s failed, Error: %s\n", error);
                tempExrData = NULL;
                loaded = false;
                return false;
            }
            format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        else if (type == S3_IMAGE_HDR)
        {
            int numComponents;
            // desired 4 channels
            tempHdrData = stbi_loadf(filePath.c_str(), &tempWidth, &tempHeight, &numComponents, 4);

            if (!tempHdrData)
            {
                s3Log::error("s3ImageDecoder load:%s failed\n");
                return false;
            }
            format = DXGI_FORMAT_R32G32B32_FLOAT;
        }

        // most detailed texture
        if (i == 0)
        {
            width = tempWidth;
            height = tempHeight;

            exrData = tempExrData;
            hdrData = tempHdrData;
            pngData.assign(tempPngData.begin(), tempPngData.end());

            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.Format = format;
        }

        // Bind Data to Texture2D
        ZeroMemory(&initialData[i], sizeof(D3D11_SUBRESOURCE_DATA));
        initialData[i].SysMemSlicePitch = 0;

        if (type == S3_IMAGE_PNG)
        {
            initialData[i].pSysMem = tempPngData.data();
            initialData[i].SysMemPitch = sizeof(uint8) * 4 * tempWidth;
        }
        else if (type == S3_IMAGE_EXR)
        {
            initialData[i].pSysMem = tempExrData;
            initialData[i].SysMemPitch = sizeof(float32) * 4 * tempWidth;
        }
        else if (type == S3_IMAGE_HDR)
        {
            initialData[i].pSysMem = tempHdrData;
            initialData[i].SysMemPitch = sizeof(float32) * 4 * tempWidth;
        }

        s3Log::success("Image:%s loaded Successfully\n", filePath.c_str());
    }

    HRESULT hr = device->CreateTexture2D(&textureDesc, initialData.data(), &texture2d);
    if (FAILED(hr))
    {
        s3Log::error("Failed to Create Texture2D\n");
        loaded = false;
        return false;
    }

    // Create texture's relative shader resource view
    if (!createSRV(format, (uint32)mipFilesNames.size(), device, texture2d, textureSRV))
    {
        loaded = false;
        return false;
    }

    // Sampler State for texture sampling
    if (!createSampler(device, samplerState))
    {
        loaded = false;
        return false;
    }

    loaded = true;
    return true;
}

t3Vector4f s3ImageDecoder::getColor(int32 x, int32 y)
{
    if (!loaded || x < 0 || y < 0)
    {
        s3Log::error("s3ImageDecoder not loaded yet\n");
        return t3Vector4f();
    }

    if (type == S3_IMAGE_PNG)
    {
        int32 index = 4 * (x + y * width);
        return t3Vector4f(pngData[index + 0] / 255.0f,
            pngData[index + 1] / 255.0f,
            pngData[index + 2] / 255.0f,
            pngData[index + 3] / 255.0f);
    }
    else if (type == S3_IMAGE_EXR)
    {
        int32 index = 4 * (x + y * width);
        return t3Vector4f(exrData[index + 0],
            exrData[index + 1],
            exrData[index + 2],
            exrData[index + 3]);
    }
    else if (type == S3_IMAGE_HDR)
    {
        int32 index = 3 * (x + y * width);
        return t3Vector4f(hdrData[index + 0],
            hdrData[index + 1],
            hdrData[index + 2],
            1.0f);
    }
    else
        return t3Vector4f();
}

int32 s3ImageDecoder::getWidth() const
{
    return width;
}

int32 s3ImageDecoder::getHeight() const
{
    return height;
}

ID3D11Texture2D * s3ImageDecoder::getTexture2D()
{
    return texture2d;
}

ID3D11ShaderResourceView * s3ImageDecoder::getShaderResouceView()
{
    return textureSRV;
}

ID3D11SamplerState * s3ImageDecoder::getSamplerState()
{
    return samplerState;
}

s3ImageType s3ImageDecoder::getImageType()
{
    return type;
}

std::vector<t3Vector4f>& s3ImageDecoder::getImageData()
{
    return imageData;
}

std::vector<unsigned char>& s3ImageDecoder::getPNGData()
{
    return pngData;
}

float32 *& s3ImageDecoder::getEXRData()
{
    return exrData;
}

float32 *& s3ImageDecoder::getHDRData()
{
    return hdrData;
}

bool s3ImageDecoder::isLoaded() const
{
    return loaded;
}
