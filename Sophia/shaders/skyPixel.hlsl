struct input
{
    float2 texCoord : TEXCOORD0;
};

struct s3Ray
{
    float3 origin;
    float3 direction;
};

struct s3Sphere
{
    float3 origin;
    float radius;
};

cbuffer skyCB
{
    // For computing perspective rays
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;

    // atmos + earth's origin
    float3 sphereOrigin;
    matrix cameraToWorld;
};

bool solveQuadratic(float A, float B, float C, inout float t0, inout float t1)
{
    // --!Copied from Mitsuba(util.cpp 447)
    /* Linear case */
    if (A == 0)
    {
        if (B != 0)
        {
            t0 = t1 = -C / B;
            return true;
        }
        return false;
    }

    float discrim = B * B - 4.0f * A * C;

    /* Leave if there is no solution */
    if (discrim < 0)
        return false;

    float temp, sqrtDiscrim = sqrt(discrim);

    /* Numerically stable version of (-b (+/-) sqrtDiscrim) / (2 * a)
    *
    * Based on the observation that one solution is always
    * accurate while the other is not. Finds the solution of
    * greater magnitude which does not suffer from loss of
    * precision and then uses the identity x1 * x2 = c / a
    */
    // 通过B的状态求得更大的temp来避免出现精度丢失问题
    if (B < 0)
        temp = -0.5f * (B - sqrtDiscrim);
    else
        temp = -0.5f * (B + sqrtDiscrim);

    t0 = temp / A;
    t1 = C / temp;

    /* Return the results so that t0 < t1 */
    if (t0 > t1)
    {
        float t = t0;
        t0 = t1;
        t1 = t;
    }

    return true;
}

// world space
bool raySphereIntersect(s3Ray r, s3Sphere s, inout float nearT, inout float farT)
{
    float3 o = r.origin - s.origin;
    float3 d = r.direction;

    // direction为单位向量，长度为1
    float A = length(r.direction);
    float B = 2 * dot(o, r.direction);
    float C = dot(o, o) - s.radius * s.radius;

    if (!solveQuadratic(A, B, C, nearT, farT))
        return false;

    return true;
}

float4 main(input i) : SV_TARGET
{
    // convert texCoord to ndc
    float ndcX = 2 * i.texCoord.x - 1;
    float ndcY = 1 - 2 * i.texCoord.y;
        
    // correct aspect ratio
    float vx = ndcX * tanHalfFovX * canvasDistance;
    float vy = ndcY * tanHalfFovY * canvasDistance;
	
	// ray camera to world
    s3Ray r;
    r.origin = float3(0, 0, 0);
    r.origin = mul(float4(r.origin, 1.0f), cameraToWorld);
	
    float3 pixelPosition = float3(vx, vy, canvasDistance);
    pixelPosition = mul(float4(pixelPosition, 1), cameraToWorld);
    r.direction = normalize(pixelPosition - r.origin);

    // km
    s3Sphere earth, atmos;
    earth.origin = sphereOrigin;
    earth.radius = 6360;

    atmos.origin = sphereOrigin;
    atmos.radius = 6420;

    // compute incident light
    float nearT, farT;
    if (!raySphereIntersect(r, earth, nearT, farT) || farT < 0 || nearT < 0)
        return float4(1, 0, 0, 1);

    return float4(0, 0, 1, 1);

    //return float4(i.texCoord.x, i.texCoord.y, 0, 1.0f);
}