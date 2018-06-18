#pragma once
#include <t3Vector3.h>

class s3PointLight
{
public:
    s3PointLight(t3Vector3f pos, t3Vector3f color);
    ~s3PointLight();

    t3Vector3f& getPosition();
    t3Vector3f& getColor();

private:
    t3Vector3f position, color;
};