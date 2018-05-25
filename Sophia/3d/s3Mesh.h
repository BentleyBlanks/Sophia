#pragma once

#include <t3Vector3.h>
#include <t3Vector2.h>
#include <core/s3Settings.h>

class s3VertexPNT
{
public:
    s3VertexPNT();
    s3VertexPNT(t3Vector3f position, t3Vector3f normal, t3Vector2f textureCoordinate, t3Vector3f color);

    t3Vector3f position;
    t3Vector3f normal;
    t3Vector2f textureCoordinate;
    t3Vector3f color;

    // input description for position / normal / texture coordinate
    static const D3D11_INPUT_ELEMENT_DESC inputDesc[4];
};

class s3Mesh
{
public:
    void draw(ID3D11DeviceContext* deviceContext);

    static s3Mesh* createCube(ID3D11DeviceContext* deviceContext, float32 size);
    static s3Mesh* createSphere(ID3D11DeviceContext* deviceContext, float32 radius = 1, int32 tessellation = 16);
private:
    s3Mesh();
    s3Mesh(const s3Mesh& ref);
    ~s3Mesh();

    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;

    int32 indexCount;
};