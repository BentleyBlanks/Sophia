#pragma once

#include <core/s3Settings.h>

class s3Shader
{
public:
    s3Shader();
    ~s3Shader();

    bool load(ID3D11Device* device, const std::wstring& vsfilePath, const std::wstring& psfilePath);

    ID3D11VertexShader* getVertexShader();
    ID3D11PixelShader* getPixelShader();

    ID3DBlob* getPixelShaderBlob();
    ID3DBlob* getVertexShaderBlob();

private:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3DBlob* pixelShaderBlob, *vertexShaderBlob;
};