#pragma once

#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3ImageDecoder
{
public:
    s3ImageDecoder();
    ~s3ImageDecoder();

    bool load(ID3D11Device* device, const std::string& filePath);
    bool load(ID3D11Device* device, int32 width, int32 height, const std::vector<t3Vector4f>& imageData);

    t3Vector4f getColor(int32 x, int32 y);

    int32 getWidth() const;
    int32 getHeight() const;

    ID3D11Texture2D* getTexture2D();
    ID3D11ShaderResourceView* getShaderResouceView();
    ID3D11SamplerState* getSamplerState();
    s3ImageType getImageType();

    std::vector<t3Vector4f>& getImageData();
    std::vector<unsigned char>& getPNGData();
    float32*& getEXRData();
    float32*& getHDRData();

    bool isLoaded() const;

private:
    s3ImageType type;

    ID3D11Texture2D* texture2d;
    ID3D11ShaderResourceView* textureSRV;
    ID3D11SamplerState* samplerState;

    int32 width, height;
    bool loaded;

    // For different type of image
    std::vector<t3Vector4f> imageData;
    std::vector<unsigned char> pngData;
    float32 *exrData, *hdrData;
};