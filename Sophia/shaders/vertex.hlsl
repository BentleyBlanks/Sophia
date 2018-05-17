cbuffer application : register(b0)
{
    matrix projectionMatrix;
}

cbuffer frame : register(b1)
{
    matrix viewMatrix;
}

cbuffer object : register(b2)
{
    matrix worldMatrix;
}

struct input
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct output
{
    float4 color : COLOR;
    float4 position : SV_POSITION;
};

output main(input i)
{
    output o;

    o.position = float4(1, 0, 0, 1);
    o.color = float4(i.color, 1.0f);

    return o;
}