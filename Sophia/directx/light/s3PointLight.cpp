#include <directx/light/s3PointLight.h>

s3PointLight::s3PointLight(t3Vector3f pos, t3Vector3f color)
    :position(pos), color(color)
{
}

s3PointLight::~s3PointLight()
{
}

t3Vector3f & s3PointLight::getPosition()
{
    return position;
}

t3Vector3f & s3PointLight::getColor()
{
    return color;
}
