#include <core/s3ImageDecoder.h>
#include <core/log/s3Log.h>
#include <app/s3Utils.h>

#include <lodepng.h>
#include <tinyexr.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// --------------------------------------s3ImageDecoder--------------------------------------
s3ImageDecoder::s3ImageDecoder()
    : width(-1), height(-1), loaded(false), type(S3_IMAGE_ERROR), exrData(nullptr), hdrData(nullptr)
{
}

s3ImageDecoder::~s3ImageDecoder()
{
    clear();
}

bool s3ImageDecoder::load(const std::string & filePath)
{
    s3Log::debug("Image:%s Begin loading...\n", filePath.c_str());
    clear();

    // --------------------------------------Texture From File--------------------------------------
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
    }
    else if (type == S3_IMAGE_EXR)
    {
        const char* error = "";
        if (LoadEXR(&exrData, &width, &height, filePath.c_str(), &error) != 0)
        {
            s3Log::error("s3ImageDecoder load:%s failed, Error: %s\n", error);
            exrData = NULL;
            return false;
        }
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
    }

    s3Log::success("Image:%s loaded Successfully\n", filePath.c_str());
    loaded = true;

    return true;
}

bool s3ImageDecoder::load(int32 width, int32 height, const std::vector<t3Vector4f>& data)
{
    if (data.size() <= 0 || width <= 0 || height <= 0)
    {
        s3Log::warning("Image Data null\n");
        return false;
    }
    
    clear();

    this->width = width;
    this->height = height;

    // copy to local
    imageData.assign(data.begin(), data.end());

    s3Log::success("s3ImageDecoder loaded Successfully\n");
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
        int32 index = 4 * (x + y * width);
        return t3Vector4f(hdrData[index + 0],
            hdrData[index + 1],
            hdrData[index + 2],
            hdrData[index + 3]);
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

void s3ImageDecoder::clear()
{
    imageData.clear();
    pngData.clear();

    S3_SAFE_DELETE_1DARRAY(exrData);
    S3_SAFE_DELETE_1DARRAY(hdrData);
}
