#define PI 3.141592653

cbuffer cb
{
	// For computing perspective rays
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;

    matrix cameraToWorld;
};

struct s3Ray
{
    float3 origin;
    float3 direction;
};

struct input
{
    float2 texCoord : TEXCOORD;
};

float s3SphericalTheta(float3 v)
{
    return acos(clamp(v.y, -1.f, 1.f));
}

float s3SphericalPhi(float3 v)
{
    float p = atan2(v.z, v.x);
    return (p < 0.f) ? p + 2.f * PI : p;
}

Texture2D tex;
sampler textureSampler;

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
	
	// for percison
    double3 pixelPosition = float3(vx, vy, canvasDistance);
    pixelPosition = mul(float4(pixelPosition, 1.0), cameraToWorld);
    r.direction = normalize(pixelPosition - r.origin);

    float phi = s3SphericalPhi(r.direction), theta = s3SphericalTheta(r.direction);

    return tex.Sample(textureSampler, float2(phi / (2 * PI), theta / PI));
}