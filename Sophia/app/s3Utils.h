#pragma once

#include <core/s3Settings.h>
#include <core/s3Event.h>

enum s3ShaderType
{
    S3_VERTEX_SHADER,
    S3_GEOMETRY_SHADER,
    S3_HULL_SHADER,
    S3_PIXEL_SHADER
};

enum s3MouseState
{
    S3_MOUSE_PRESSED = 0,
    S3_MOUSE_RELEASED,
    S3_MOUSE_MOVED,
    S3_MOUSE_SCROLLED
};

enum s3KeyState
{
    S3_KEY_PRESSED = 0,
    S3_KEY_RELEASED
};

std::string s3MouseStateToString(s3MouseState state);

std::string s3KeyStateToString(s3KeyState state);

std::string s3MouseTypeToString(s3MouseEvent::s3ButtonType type);

std::string s3KeyTypeToString(s3KeyCode type);

std::string s3BoolToString(bool b);

std::string s3GetLatestProfile(s3ShaderType type);
