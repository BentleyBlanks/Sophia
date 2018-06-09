#include <3d/s3Mesh.h>
#include <core/s3MathHepler.h>
#include <t3Math.h>

// test
#include <core/s3Random.h>

// Helper for creating a D3D vertex or index buffer.
template<typename T>
void s3CreateBuffer(ID3D11Device* device, T const& data, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer)
{
    assert(buffer != 0);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

    bufferDesc.ByteWidth = (uint32) data.size() * sizeof(T::value_type);
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    resourceData.pSysMem = data.data();

    HRESULT hr = device->CreateBuffer(&bufferDesc, &resourceData, buffer);
    if (FAILED(hr))
    {
        throw std::exception("Failed to create buffer.");
    }
}

t3Vector3f generateRandomVector()
{
    //return t3Vector3f(s3Random::randomFloat(), s3Random::randomFloat(), s3Random::randomFloat());
    return t3Vector3f(1, 1, 1);
}

// -----------------------------------------------s3VertexPNT-----------------------------------------------
const D3D11_INPUT_ELEMENT_DESC s3VertexPNT::inputDesc[] = 
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(s3VertexPNT, position), D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(s3VertexPNT, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(s3VertexPNT, textureCoordinate), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(s3VertexPNT, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

s3VertexPNT::s3VertexPNT()
{
}

s3VertexPNT::s3VertexPNT(t3Vector3f position, t3Vector3f normal, t3Vector2f textureCoordinate, t3Vector3f color)
    :position(position), normal(normal), textureCoordinate(textureCoordinate), color(color)
{
}

// -----------------------------------------------s3Mesh-----------------------------------------------
s3Mesh* s3Mesh::createCube(ID3D11DeviceContext* deviceContext, float32 size)
{    // A cube has six faces, each one pointing in a different direction.
    const int faceCount = 6;

    static const t3Vector3f faceNormals[faceCount] =
    {
        { 0,  0,  1 },
        { 0,  0, -1 },
        { 1,  0,  0 },
        { -1,  0,  0 },
        { 0,  1,  0 },
        { 0, -1,  0 },
    };

    static const t3Vector2f textureCoordinates[4] =
    {
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 0, 0 },
    };

    std::vector<s3VertexPNT> vertices;
    std::vector<uint32> indices;

    size /= 2;

    // Create each face in turn.
    for (int32 i = 0; i < faceCount; i++)
    {
        t3Vector3f normal = faceNormals[i];

        // Get two vectors perpendicular both to the face normal and to each other.
        t3Vector3f basis = (i >= 4) ? t3Vector3f(0.0f, 0.0f, 1.0f) : t3Vector3f(0.0f, 1.0f, 0.0f);

        t3Vector3f side1 = normal.getCrossed(basis);
        t3Vector3f side2 = normal.getCrossed(side1);

        // Six indices (two triangles) per face.
        int32 vbase = (int32) vertices.size();
        indices.push_back(vbase + 0);
        indices.push_back(vbase + 1);
        indices.push_back(vbase + 2);

        indices.push_back(vbase + 0);
        indices.push_back(vbase + 2);
        indices.push_back(vbase + 3);

        // Four vertices per face.
        vertices.push_back(s3VertexPNT((normal - side1 - side2) * size, normal, textureCoordinates[0], generateRandomVector()));
        vertices.push_back(s3VertexPNT((normal - side1 + side2) * size, normal, textureCoordinates[1], generateRandomVector()));
        vertices.push_back(s3VertexPNT((normal + side1 + side2) * size, normal, textureCoordinates[2], generateRandomVector()));
        vertices.push_back(s3VertexPNT((normal + side1 - side2) * size, normal, textureCoordinates[3], generateRandomVector()));
    }

    // Create the primitive object.
    s3Mesh* mesh = new s3Mesh();

    ID3D11Device* device;
    deviceContext->GetDevice(&device);

    s3CreateBuffer(device, vertices, D3D11_BIND_VERTEX_BUFFER, &mesh->vertexBuffer);
    s3CreateBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, &mesh->indexBuffer);

    mesh->indexCount = (int32) indices.size();

    return mesh;
}

s3Mesh* s3Mesh::createSphere(ID3D11DeviceContext* deviceContext, float32 radius, int32 tessellation)
{
    std::vector<s3VertexPNT> vertices;
    std::vector<uint32> indices;

    if (tessellation < 3)
        throw std::out_of_range("tessellation parameter out of range");

    int32 verticalSegments = tessellation;
    int32 horizontalSegments = tessellation * 2;

    // Create rings of vertices at progressively higher latitudes.
    for (int32 i = 0; i <= verticalSegments; i++)
    {
        float32 v = 1 - (float)i / verticalSegments;

        float32 latitude = (i * T3MATH_PI / verticalSegments) - T3MATH_PI_DIV2;
        float32 dy, dxz;

        s3ScalarSinCos(&dy, &dxz, latitude);

        // Create a single ring of vertices at this latitude.
        for (int32 j = 0; j <= horizontalSegments; j++)
        {
            float32 u = (float)j / horizontalSegments;

            float32 longitude = j * T3MATH_2PI / horizontalSegments;
            float32 dx, dz;

            s3ScalarSinCos(&dx, &dz, longitude);

            dx *= dxz;
            dz *= dxz;

            t3Vector3f normal(dx, dy, dz);
            t3Vector2f textureCoordinate(u, v);

            vertices.push_back(s3VertexPNT(normal * radius, normal, textureCoordinate, generateRandomVector()));
        }
    }

    // Fill the index buffer with triangles joining each pair of latitude rings.
    int32 stride = horizontalSegments + 1;

    for (int32 i = 0; i < verticalSegments; i++)
    {
        for (int32 j = 0; j <= horizontalSegments; j++)
        {
            int32 nextI = i + 1;
            int32 nextJ = (j + 1) % stride;

            indices.push_back(i * stride + nextJ);
            indices.push_back(nextI * stride + j);
            indices.push_back(i * stride + j);

            indices.push_back(nextI * stride + nextJ);
            indices.push_back(nextI * stride + j);
            indices.push_back(i * stride + nextJ);
        }
    }

    // Create the primitive object.
    s3Mesh* mesh = new s3Mesh();

    ID3D11Device* device;
    deviceContext->GetDevice(&device);

    s3CreateBuffer(device, vertices, D3D11_BIND_VERTEX_BUFFER, &mesh->vertexBuffer);
    s3CreateBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, &mesh->indexBuffer);

    mesh->indexCount = (int32) indices.size();

    return mesh;
}

void s3Mesh::draw(ID3D11DeviceContext * deviceContext) const
{
    uint32 vertexStride = sizeof(s3VertexPNT);
    uint32 offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}

s3Mesh::s3Mesh() :indexCount(0)
{
}

s3Mesh::s3Mesh(const s3Mesh & ref)
{
}

s3Mesh::~s3Mesh()
{
}

void s3Mesh::setObjectToWorld(t3Matrix4x4 objectToWorld)
{
    this->objectToWorld = objectToWorld;
    this->worldToObject = objectToWorld.getInverse();
}

void s3Mesh::setWoldToObject(t3Matrix4x4 worldToObject)
{
    this->worldToObject = worldToObject;
    this->objectToWorld = worldToObject.getInverse();
}

t3Matrix4x4 s3Mesh::getObjectToWorld() const
{
    return objectToWorld;
}

t3Matrix4x4 s3Mesh::getWoldToObject() const
{
    return worldToObject;
}
