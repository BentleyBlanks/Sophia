struct input
{
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

Texture2D textureTest : register(t0);
sampler textureSampler : register(s0);

float4 main(input i) : SV_TARGET
{
    //i.normal.x = abs(i.normal.x);
    //i.normal.y = abs(i.normal.x);
    //i.normal.z = abs(i.normal.x);
    //return i.color;
    //return float4(i.normal, 1.0f);

    return textureTest.Sample(textureSampler, i.texCoord);
}