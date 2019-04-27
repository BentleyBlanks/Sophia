struct input
{
    float2 texCoord : TEXCOORD;
};

Texture2D tex;
sampler textureSampler;

float4 main(input i) : SV_TARGET
{
    return tex.Sample(textureSampler, i.texCoord);
}