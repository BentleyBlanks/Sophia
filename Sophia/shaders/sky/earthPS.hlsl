struct input
{
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

Texture2D earthGround;
sampler textureSampler;

float4 main(input i) : SV_TARGET
{
	
    return earthGround.Sample(textureSampler, i.texCoord);
    //return float4(0.5f, 0.5f, 0.5f, 1);
}