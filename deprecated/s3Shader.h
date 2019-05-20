#pragma once

#include <core/s3Settings.h>

class s3Shader
{
public:
    s3Shader();
    ~s3Shader();

    bool load(const std::wstring& vsfilePath, const std::wstring& psfilePath);

    ID3D11VertexShader* getVertexShader();
    ID3D11PixelShader* getPixelShader();

    ID3DBlob* getPixelShaderBlob();
    ID3DBlob* getVertexShaderBlob();

    ID3D11InputLayout* getInputLayout();

private:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3DBlob* pixelShaderBlob, *vertexShaderBlob;
    ID3D11InputLayout* inputLayout;
};