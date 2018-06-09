cbuffer cb
{
	// camera
    matrix mvp;
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
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float4 position : SV_POSITION;
};

output main(input i)
{
    output o;

    // column-major matrix(transpose of the matrix on CPU)
    o.position = mul(float4(i.position, 1.0f), mvp);
    o.texCoord = i.texCoord;
    o.color = float4(i.color, 1.0f);

    return o;
}