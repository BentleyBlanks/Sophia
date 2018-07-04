cbuffer cb
{
	// camera
    matrix projection, view, model, normalM;
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
    float4 fragPos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 position : SV_POSITION;
};

output main(input i)
{
    output o;

    // column-major matrix(transpose of the matrix on CPU)
    o.position = mul(mul(mul(float4(i.position, 1.0f), model), view), projection);
    o.normal = mul(float4(i.normal, 1.0f), normalM).xyz;
    o.texCoord = i.texCoord;
    o.fragPos = o.position;

    return o;
}