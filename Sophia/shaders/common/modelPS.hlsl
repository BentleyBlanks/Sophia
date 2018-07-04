struct input
{
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

float4 main(input i) : SV_TARGET
{
    return float4(i.normal.xyz, 1);
}