#include <core/s3MathHepler.h>
#include <t3Math.h>

t3Matrix4x4 rightToLeftHand(1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1);

t3Matrix4x4 makeRotationMatrix(float angle, s3Axis axis)
{
    float cosTheta = t3Math::cosDeg(angle), sinTheta = t3Math::sinDeg(angle);
    switch (axis)
    {
    case S3_X:
        return t3Matrix4x4(1, 0, 0, 0,
                           0, cosTheta, sinTheta, 0,
                           0, -sinTheta, cosTheta, 0,
                           0, 0, 0, 1);
    case S3_Y:
        return t3Matrix4x4(cosTheta, -sinTheta, 0, 0,
                           0, 1, 0, 0,
                           sinTheta, 0, cosTheta, 0,
                           0, 0, 0, 1);
    case S3_Z:
        return t3Matrix4x4(cosTheta, sinTheta, 0, 0,
                           -sinTheta, cosTheta, 0, 0,
                           0, 0, 0, 0,
                           0, 0, 0, 1);
    default:
        return t3Matrix4x4::newIdentityMatrix();
    }
}

t3Matrix4x4 makeRotationMatrix(float theta, const t3Vector3f & axis)
{
    t3Matrix4x4 a;
    float u = axis.x;
    float v = axis.y;
    float w = axis.z;

    a._mat[0][0] = t3Math::cosDeg(theta) + (u * u) * (1 - t3Math::cosDeg(theta));
    a._mat[0][1] = u * v * (1 - t3Math::cosDeg(theta)) + w * t3Math::sinDeg(theta);
    a._mat[0][2] = u * w * (1 - t3Math::cosDeg(theta)) - v * t3Math::sinDeg(theta);
    a._mat[0][3] = 0;

    a._mat[1][0] = u * v * (1 - t3Math::cosDeg(theta)) - w * t3Math::sinDeg(theta);
    a._mat[1][1] = t3Math::cosDeg(theta) + v * v * (1 - t3Math::cosDeg(theta));
    a._mat[1][2] = w * v * (1 - t3Math::cosDeg(theta)) + u * t3Math::sinDeg(theta);
    a._mat[1][3] = 0;

    a._mat[2][0] = u * w * (1 - t3Math::cosDeg(theta)) + v * t3Math::sinDeg(theta);
    a._mat[2][1] = v * w * (1 - t3Math::cosDeg(theta)) - u * t3Math::sinDeg(theta);
    a._mat[2][2] = t3Math::cosDeg(theta) + w * w * (1 - t3Math::cosDeg(theta));
    a._mat[2][3] = 0;

    a._mat[3][0] = 0;
    a._mat[3][1] = 0;
    a._mat[3][2] = 0;
    a._mat[3][3] = 1;

    return a;
}

t3Matrix4x4 makeTranslationMatrix(const t3Vector3f& t)
{
    t3Matrix4x4 a;
    a.makeTranslationMatrix(t);
    return a;
}

t3Matrix4x4 makeScaleMatrix(const t3Vector3f& s)
{
    t3Matrix4x4 a;
    a.makeScaleMatrix(s);
    return a;
}
