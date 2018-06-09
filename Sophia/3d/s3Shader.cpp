#include <3d/s3Shader.h>
#include <app/s3Utils.h>
#include <core/log/s3Log.h>
#include <core/s3Settings.h>

s3Shader::s3Shader() : 
    vertexShader(nullptr), 
    pixelShader(nullptr), 
    pixelShaderBlob(nullptr), 
    vertexShaderBlob(nullptr)
{
}

s3Shader::~s3Shader()
{
    S3_SAFE_RELEASE(vertexShaderBlob);
    S3_SAFE_RELEASE(pixelShaderBlob);

    S3_SAFE_RELEASE(vertexShader);
    S3_SAFE_RELEASE(pixelShader);
}

bool s3Shader::load(ID3D11Device* device, const std::wstring& vsFilePath, const std::wstring & psFilePath)
{
    ID3DBlob* errorBlob = nullptr;

    std::string profile = s3GetLatestProfile(s3ShaderType::S3_VERTEX_SHADER);
    uint32 flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    // ---------------------------------vertex shader---------------------------------
    HRESULT hr = D3DCompileFromFile(
        vsFilePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        profile.c_str(),
        flags,
        0,
        &vertexShaderBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            s3Log::error("Vertex Shader Compile Failed: %s", (char*)errorBlob->GetBufferPointer());
            S3_SAFE_RELEASE(vertexShaderBlob);
            S3_SAFE_RELEASE(errorBlob);
            return false;
        }
    }

    hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader);

    if (FAILED(hr))
    {
        s3Log::error("Vertex Shader Create Failed\n");
        return false;
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
        &pixelShaderBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            s3Log::error("Pixel Shader Compile Failed: %s", (char*)errorBlob->GetBufferPointer());
            S3_SAFE_RELEASE(pixelShaderBlob);
            S3_SAFE_RELEASE(errorBlob);
            return false;
        }
    }

    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &pixelShader);

    if (FAILED(hr))
    {
        s3Log::error("Pixel Shader Create Failed\n");
        return false;
    }

    S3_SAFE_RELEASE(errorBlob);
    return true;
}

ID3D11VertexShader* s3Shader::getVertexShader()
{
    return vertexShader;
}

ID3D11PixelShader* s3Shader::getPixelShader()
{
    return pixelShader;
}

ID3DBlob* s3Shader::getPixelShaderBlob()
{
    return pixelShaderBlob;
}

ID3DBlob* s3Shader::getVertexShaderBlob()
{
    return vertexShaderBlob;
}
