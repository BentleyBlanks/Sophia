struct input
{
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

float4 main(input i) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}