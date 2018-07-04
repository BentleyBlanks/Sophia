#pragma once

#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3ImageDecoder
{
public:
    s3ImageDecoder();
    ~s3ImageDecoder();

    bool load(int32 width, int32 height, const std::vector<t3Vector4f>& imageData);
    bool load(const std::string& filePath);

    t3Vector4f getColor(int32 x, int32 y);

    // return most detailed mipmap's size if existed
    int32 getWidth() const;
    int32 getHeight() const;

    s3ImageType getImageType();

    std::vector<t3Vector4f>& getImageData();
    std::vector<unsigned char>& getPNGData();
    float32*& getEXRData();
    float32*& getHDRData();

    bool isLoaded() const;

private:
    void clear();

    s3ImageType type;

    int32 width, height;
    bool loaded;

    // For different type of image
    std::vector<t3Vector4f> imageData;
    std::vector<unsigned char> pngData;
    float32 *exrData, *hdrData;

    int32 mipLevels;
};