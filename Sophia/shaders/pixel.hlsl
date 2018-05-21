struct input
{
    float4 color : COLOR;
};

float4 main(input i) : SV_TARGET
{
    return i.color;
}