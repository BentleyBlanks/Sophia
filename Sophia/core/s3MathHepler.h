#pragma once
#include <t3Matrix4x4.h>
#include <t3Vector3.h>

enum s3Axis
{
    S3_X,
    S3_Y,
    S3_Z
};

struct s3Ray
{
    t3Vector3f origin;
    t3Vector3f direction;
};

struct s3Sphere
{
    t3Vector3f origin;
    float radius;
};

// For bad implementation of matrix
t3Matrix4x4 makeRotationMatrix(float angle, s3Axis axis);

t3Matrix4x4 makeRotationMatrix(float angle, const t3Vector3f& axis);

t3Matrix4x4 makeRotationMatrix(float angle, const t3Vector3f& origin, const t3Vector3f& axis);

t3Matrix4x4 makeTranslationMatrix(const t3Vector3f& t);

t3Matrix4x4 makeScaleMatrix(const t3Vector3f& s);

void s3ScalarSinCos(float* pSin, float* pCos, float  Value);

bool s3SolveQuadratic(float A, float B, float C, float* t0, float* t1);

// world space
bool s3RaySphereIntersect(s3Ray r, s3Sphere s, float* nearT, float* farT);