cbuffer cb
{
    matrix projection;
    matrix view;
    matrix model;
    matrix normalM;
}

struct input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR;
};

struct output
{
    float3 fragPos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 position : SV_POSITION;
};

output main(input i)
{
    output o;

    // column-major matrix(transpose of the matrix on CPU)
    float3x3 model3x3 = model;
    o.position = mul(mul(mul(float4(i.position, 1.0f), model), view), projection);
    o.normal = mul(float3(i.normal), model3x3);
    o.fragPos = mul(float4(i.position, 1.0f), model).xyz;
    o.texCoord = i.texCoord;

    return o;
}