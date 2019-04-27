
struct input
{
    float2 texCoord : TEXCOORD0;
};

Texture2D lut;
sampler textureSampler;

float4 main(input i) : SV_TARGET
{	
    float c = lut.Sample(textureSampler, i.texCoord).y;
    //c = c > 1.0f ? 1.0f : c;
    return float4(c, c, c, 1);
}