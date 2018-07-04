#pragma once

#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3Texture
{
public:
    s3Texture();
    ~s3Texture();

    // as texture from data
    bool load(int32 width, int32 height, std::vector<t3Vector4f> data);
    bool load(int32 width, int32 height, std::vector<unsigned char> data);
    bool load(int32 width, int32 height, float32* data);

    // data as mipmap
    bool load(int32 width, int32 height, std::vector<float32*> data);
    bool load(int32 width, int32 height, std::vector<std::vector<t3Vector4f>> data);
    bool load(int32 width, int32 height, std::vector<std::vector<unsigned char>> data);

    // as render target
    bool createRT(int32 width, int32 height);

    ID3D11Texture2D*& getTexture2D();
    ID3D11ShaderResourceView*& getShaderResouceView();
    ID3D11SamplerState*& getSamplerState();
    ID3D11RenderTargetView*& getRenderTargetView();

private:
    ID3D11Texture2D* texture2d;

    // as resource
    ID3D11ShaderResourceView* srv;
    ID3D11SamplerState* samplerState;

    // as render target
    ID3D11RenderTargetView* renderTargetView;

    // size
    int32 width, height;
};