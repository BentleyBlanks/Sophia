struct output
{
    float2 texCoord : TEXCOORD0;
    float4 position : SV_POSITION;
};

output main(uint id : SV_VertexID)
{
    output o;

    o.texCoord = double2((id << 1) & 2, id & 2);
    o.position = double4(o.texCoord * double2(2, -2) + double2(-1, 1), 0, 1);

    return o;
}