#pragma once

#include <core/s3Settings.h>

enum s3ShaderType
{
    S3_VERTEX_SHADER,
    S3_GEOMETRY_SHADER,
    S3_HULL_SHADER,
    S3_PIXEL_SHADER
};

std::string s3GetLatestProfile(s3ShaderType type);