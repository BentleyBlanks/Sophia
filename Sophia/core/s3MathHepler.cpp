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

t3Matrix4x4 makeRotationMatrix(float theta, const t3Vector3f & origin, const t3Vector3f & axis)
{
    float a = origin.x;
    float b = origin.y;
    float c = origin.z;

    float u = axis.x;
    float v = axis.y;
    float w = axis.z;

    float uu = u * u;
    float uv = u * v;
    float uw = u * w;
    float vv = v * v;
    float vw = v * w;
    float ww = w * w;
    float au = a * u;
    float av = a * v;
    float aw = a * w;
    float bu = b * u;
    float bv = b * v;
    float bw = b * w;
    float cu = c * u;
    float cv = c * v;
    float cw = c * w;

    float costheta = t3Math::cosDeg(theta);
    float sintheta = t3Math::sinDeg(theta);

    t3Matrix4x4 matrix;

    matrix._mat[0][0] = uu + (vv + ww) * costheta;
    matrix._mat[0][1] = uv * (1 - costheta) + w * sintheta;
    matrix._mat[0][2] = uw * (1 - costheta) - v * sintheta;
    matrix._mat[0][3] = 0;

    matrix._mat[1][0] = uv * (1 - costheta) - w * sintheta;
    matrix._mat[1][1] = vv + (uu + ww) * costheta;
    matrix._mat[1][2] = vw * (1 - costheta) + u * sintheta;
    matrix._mat[1][3] = 0;

    matrix._mat[2][0] = uw * (1 - costheta) + v * sintheta;
    matrix._mat[2][1] = vw * (1 - costheta) - u * sintheta;
    matrix._mat[2][2] = ww + (uu + vv) * costheta;
    matrix._mat[2][3] = 0;

    matrix._mat[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
    matrix._mat[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
    matrix._mat[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
    matrix._mat[3][3] = 1;

    return matrix;
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

void s3ScalarSinCos(float * pSin, float * pCos, float Value)
{
    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = T3MATH_1DIV_2PI*Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - T3MATH_2PI*quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > T3MATH_PI_DIV2)
    {
        y = T3MATH_PI - y;
        sign = -1.0f;
    }
    else if (y < -T3MATH_PI_DIV2)
    {
        y = -T3MATH_PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    // 10-degree minimax approximation
    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *pCos = sign*p;
}

