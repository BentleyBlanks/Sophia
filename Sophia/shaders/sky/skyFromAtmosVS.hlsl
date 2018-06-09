cbuffer cb
{
	// camera
    matrix mvp;
    float3 cameraPosition;
    float cameraHeight, cameraHeight2;
	
    float samples;

    float3 sphereOrigin;
    float atmosRadius, atmosRadius2;
    float earthRadius, earthRadius2;

    float KrSun, KmSun;
    float3 lightDirection;
    float Kr, Kr4PI;
    float Km, Km4PI;
	// 1 / (atmosRadius - earthRadius)
    float scale;
	// 1 / pow(wavelength, 4) for RGB
    float3 invWavelengthPow4;
    float3 sunIntensity;
    float g;
}

struct input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR;
};

struct output
{
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float4 position : SV_POSITION;
};

output main(input i)
{
    output o;

    // column-major matrix(transpose of the matrix on CPU)
    o.position = mul(float4(i.position, 1.0f), mvp);
    o.texCoord = i.texCoord;
    o.color = float4(i.color, 1.0f);

    return o;
}