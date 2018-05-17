#include <app/s3Utils.h>
#include <app/s3Renderer.h>
#include <core/log/s3Log.h>

std::string s3GetLatestProfileVertexShader(D3D_FEATURE_LEVEL featureLevel)
{
    switch(featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "vs_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "vs_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "vs_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "vs_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "vs_4_0_level_9_1";
    }
    break;
    } // switch( featureLevel )

    return "";
}

std::string s3GetLatestProfilePixelShader(D3D_FEATURE_LEVEL featureLevel)
{
    switch(featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "ps_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "ps_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "ps_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "ps_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "ps_4_0_level_9_1";
    }
    break;
    }
    return "";
}

std::string s3GetLatestProfile(s3ShaderType type)
{
    ID3D11Device* device = s3Renderer::get().getDevice();
    if(!device)
    {
        s3Log::warning("s3GetLatestProfile():Need to initialze the renderer first\n");
        return "";
    }

    D3D_FEATURE_LEVEL featureLevel = device->GetFeatureLevel();

    switch(type)
    {
    case S3_VERTEX_SHADER:
        return s3GetLatestProfileVertexShader(featureLevel);
    case S3_PIXEL_SHADER:
        return s3GetLatestProfilePixelShader(featureLevel);
    case S3_GEOMETRY_SHADER:
    case S3_HULL_SHADER:
    default:
        return "";
    }
}