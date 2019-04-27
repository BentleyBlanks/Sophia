#include <directx/s3Shader.h>
#include <directx/grpahics/s3Renderer.h>
#include <app/s3Utils.h>
#include <core/log/s3Log.h>
#include <core/s3Settings.h>
#include <3d/s3Mesh.h>

s3Shader::s3Shader() : 
    vertexShader(nullptr), 
    pixelShader(nullptr), 
    pixelShaderBlob(nullptr), 
    vertexShaderBlob(nullptr),
    inputLayout(nullptr)
{
}

s3Shader::~s3Shader()
{
    S3_SAFE_RELEASE(vertexShaderBlob);
    S3_SAFE_RELEASE(pixelShaderBlob);

    S3_SAFE_RELEASE(vertexShader);
    S3_SAFE_RELEASE(pixelShader);
}

bool s3Shader::load(const std::wstring& vsFilePath, const std::wstring & psFilePath)
{
    ID3D11Device* device = s3Renderer::get().getDevice();
    ID3DBlob* errorBlob  = nullptr;

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

    // Vertex PNT Input layout 
    hr = device->CreateInputLayout(
        s3VertexPNT::inputDesc,
        _countof(s3VertexPNT::inputDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &inputLayout);

    if (FAILED(hr))
    {
        s3Log::error("Input Layout Create Failed\n");
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

ID3D11InputLayout * s3Shader::getInputLayout()
{
    return inputLayout;
}
