#pragma once
#include <t3Matrix4x4.h>

enum s3Axis
{
    S3_X,
    S3_Y,
    S3_Z
};

// For bad implementation of matrix
t3Matrix4x4 makeRotationMatrix(float angle, s3Axis axis);

t3Matrix4x4 makeRotationMatrix(float angle, const t3Vector3f& axis);

t3Matrix4x4 makeRotationMatrix(float angle, const t3Vector3f& origin, const t3Vector3f& axis);

t3Matrix4x4 makeTranslationMatrix(const t3Vector3f& t);

t3Matrix4x4 makeScaleMatrix(const t3Vector3f& s);

void s3ScalarSinCos(float* pSin, float* pCos, float  Value);