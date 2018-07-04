#pragma once

#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
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
    s3Mesh();
    s3Mesh(const s3Mesh& ref);
    ~s3Mesh();

    // load the model to entire mesh
    bool load(const char* fileName);

    void draw() const;

    void setObjectToWorld(t3Matrix4x4 objectToWorld);
    void setWoldToObject(t3Matrix4x4 worldToObject);

    t3Matrix4x4 getObjectToWorld() const;
    t3Matrix4x4 getWoldToObject() const;

    static s3Mesh* createCube(float32 size);
    static s3Mesh* createSphere(float32 radius = 1, int32 tessellation = 16);

private:
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;

    int32 indexCount;

    t3Matrix4x4 objectToWorld, worldToObject;
};