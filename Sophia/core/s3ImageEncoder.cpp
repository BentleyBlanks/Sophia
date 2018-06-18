#include <core/s3ImageEncoder.h>
#include <app/s3Utils.h>
#include <core/log/s3Log.h>

#include <lodepng.h>
#include <tinyexr.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

bool s3SaveExr(const float* rgb, int width, int height, const char* fileName)
{
    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = 3;

    std::vector<float> images[3];
    images[0].resize(width * height);
    images[1].resize(width * height);
    images[2].resize(width * height);

    // Split RGBRGBRGB... into R, G and B layer
    for (int i = 0; i < width * height; i++) {
        images[0][i] = rgb[3 * i + 0];
        images[1][i] = rgb[3 * i + 1];
        images[2][i] = rgb[3 * i + 2];
    }

    float* image_ptr[3];
    image_ptr[0] = &(images[2].at(0)); // B
    image_ptr[1] = &(images[1].at(0)); // G
    image_ptr[2] = &(images[0].at(0)); // R

    image.images = (unsigned char**)image_ptr;
    image.width = width;
    image.height = height;

    header.num_channels = 3;
    header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    // Must be (A)BGR order, since most of EXR viewers expect this channel order.
    strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
    strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
    strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

    header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
    }

    const char* err = NULL; // or nullptr in C++11 or later.
    int ret = SaveEXRImageToFile(&image, &header, fileName, &err);
    if (ret != TINYEXR_SUCCESS) {
        fprintf(stderr, "Save EXR err: %s\n", err);
        FreeEXRErrorMessage(err); // free's buffer for an error message 
        return false;
    }
    s3Log::print("Saved exr file. [ %s ] \n", fileName);

    //free(rgb);

    free(header.channels);
    free(header.pixel_types);
    free(header.requested_pixel_types);

    return true;
}

s3ImageEncoder::s3ImageEncoder(int32 width, int32 height, s3ImageType type)
    : width(width), height(height), type(type)
{
    switch (type)
    {
    case S3_IMAGE_PNG:
        pngData.resize(width * height * 4);
        break;
    case S3_IMAGE_EXR:
        exrData = new float[width * height * 3];
        break;
    case S3_IMAGE_HDR:
        hdrData = new float[width * height * 4];
        break;
    }
}

s3ImageEncoder::~s3ImageEncoder()
{
    pngData.clear();
    S3_SAFE_DELETE_1DARRAY(exrData);
    S3_SAFE_DELETE_1DARRAY(hdrData);
}

void s3ImageEncoder::setColor(int32 x, int32 y, const t3Vector4f & L)
{
    switch (type)
    {
    case S3_IMAGE_PNG:
        pngData[(x + y * width) * 4 + 0] = (uint8)(L.x * 255.0f);
        pngData[(x + y * width) * 4 + 1] = (uint8)(L.y * 255.0f);
        pngData[(x + y * width) * 4 + 2] = (uint8)(L.z * 255.0f);
        pngData[(x + y * width) * 4 + 3] = (uint8)(L.w * 255.0f);
        break;
    case S3_IMAGE_EXR:
        exrData[(x + y * width) * 3 + 0] = L.x;
        exrData[(x + y * width) * 3 + 1] = L.y;
        exrData[(x + y * width) * 3 + 2] = L.z;
        //exrData[(x + y * width) * 4 + 3] = L.w;
        break;
    case S3_IMAGE_HDR:
        hdrData[(x + y * width) * 3 + 0] = L.x;
        hdrData[(x + y * width) * 3 + 1] = L.y;
        hdrData[(x + y * width) * 3 + 2] = L.z;
        //hdrData[(x + y * width) * 4 + 3] = L.w;
        break;
    default:
        s3Log::error("s3ImageEncoder image: %s saved failed, wrong image type\n", type);
        break;
    }
}

bool s3ImageEncoder::write(const std::string& filePath)
{
    FILE* f = NULL;

    switch (type)
    {
    case S3_IMAGE_PNG:
    {
        unsigned error = lodepng::encode(filePath, pngData, width, height);
        if (error)
        {
            s3Log::error("s3ImageEncoder image: %s saved failed, Error: %s\n", filePath.c_str(), lodepng_error_text(error));
            return false;
        }

        s3Log::success("s3ImageEncoder image: %s saved succeed\n", filePath.c_str());
        return true;
    }
    case S3_IMAGE_EXR:
    {
        if (!s3SaveExr(exrData, width, height, filePath.c_str()))
            return false;

        s3Log::success("s3ImageEncoder image: %s saved successfully", filePath.c_str());
        return true;
    }

    case S3_IMAGE_HDR:
    default:
        s3Log::error("s3ImageEncoder image: %s saved failed, wrong image type\n", filePath.c_str());
        return false;
    }
}
