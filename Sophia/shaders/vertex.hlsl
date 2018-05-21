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

    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    o.position = mul(mvp, float4(i.position, 1.0f));
    o.color = float4(i.color, 1.0f);

    return o;
}