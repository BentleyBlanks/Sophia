struct output
{
    float2 texCoord : TEXCOORD0;
    float4 position : SV_POSITION;
};

output main(uint id : SV_VertexID)
{
    output o;

    o.texCoord = float2((id << 1) & 2, id & 2);
    o.position = float4(o.texCoord * float2(2, -2) + float2(-1, 1), 1.0f, 1);

    return o;
}