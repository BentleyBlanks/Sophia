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
    matrix modelMatrix;
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

    // column-major matrix(transpose of the matrix on CPU)
    matrix mvp = mul(mul(modelMatrix, viewMatrix), projectionMatrix);
    o.position = mul(float4(i.position, 1.0f), mvp);

    // row-major matrix
    //matrix mvp = mul(projectionMatrix, mul(viewMatrix, modelMatrix));
    //o.position = mul(mvp, float4(i.position, 1.0f));

    o.color = float4(i.color, 1.0f);

    return o;
}