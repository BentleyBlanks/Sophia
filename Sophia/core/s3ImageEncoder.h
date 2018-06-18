#pragma once
#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3ImageEncoder
{
public:
    s3ImageEncoder(int32 width, int32 height, s3ImageType type);
    ~s3ImageEncoder();

    void setColor(int32 x, int32 y, const t3Vector4f& L);

    bool write(const std::string& filePath);

private:
    // imageData
    float32 *exrData, *hdrData;
    std::vector<uint8> pngData;

    int32 width, height;
    s3ImageType type;
};