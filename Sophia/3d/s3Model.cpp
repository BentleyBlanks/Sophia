#include <3d/s3Model.h>
#include <3d/s3Mesh.h>
#include <app/s3Renderer.h>
#include <core/s3MathHepler.h>
#include <core/log/s3Log.h>
#include <t3Math.h>

#include <tiny_obj_loader.h>

// Helper for creating a D3D vertex or index buffer.
template<typename T>
void s3CreateBuffer(ID3D11Device* device, T const& data, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer)
{
    assert(buffer != 0);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

    bufferDesc.ByteWidth           = (uint32)data.size() * sizeof(T::value_type);
    bufferDesc.BindFlags           = bindFlags;
    bufferDesc.CPUAccessFlags      = 0;
    bufferDesc.MiscFlags           = 0;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.Usage               = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    resourceData.pSysMem = data.data();

    HRESULT hr = device->CreateBuffer(&bufferDesc, &resourceData, buffer);
    if (FAILED(hr))
    {
        throw std::exception("Failed to create buffer.");
    }
}

s3Model::s3Model()
{
}

s3Model::~s3Model()
{
    clear();
}

bool s3Model::load(const char* filePath)
{
    ID3D11DeviceContext* deviceContext = s3Renderer::get().getDeviceContext();

    s3Log::debug("Parsering model file: %s...\n", filePath);

    // TinyObjLoader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath);

    if (!err.empty())
        s3Log::warning("Warning: %s\n", err.c_str());

    if (!ret)
    {
        s3Log::error("Model loading failed\n");
        return false;
    }

    for (const auto& shape : shapes)
    {
        s3Mesh* mesh = new s3Mesh();

        // Mesh
        std::vector<s3VertexPNT> vertices;
        std::vector<uint32> indices;

        for (const auto& index : shape.mesh.indices)
        {
            s3VertexPNT vertex;

            vertex.position = t3Vector3f(attrib.vertices[3 * index.vertex_index + 0],
                                         attrib.vertices[3 * index.vertex_index + 1],
                                         attrib.vertices[3 * index.vertex_index + 2]);

            vertex.normal = t3Vector3f(attrib.normals[3 * index.normal_index + 0],
                                       attrib.normals[3 * index.normal_index + 1],
                                       attrib.normals[3 * index.normal_index + 2]);

            vertex.textureCoordinate = t3Vector2f(attrib.texcoords[2 * index.texcoord_index + 0],
                                                  attrib.texcoords[2 * index.texcoord_index + 1]);

            vertices.push_back(vertex);
            indices.push_back((int32)indices.size());
        }

        mesh->name = shape.name;
        mesh->indexCount = (int32)indices.size();

        ID3D11Device* device;
        deviceContext->GetDevice(&device);

        s3CreateBuffer(device, vertices, D3D11_BIND_VERTEX_BUFFER, &mesh->vertexBuffer);
        s3CreateBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, &mesh->indexBuffer);

        meshes.push_back(mesh);
    }

    s3Log::success("Model loaded successfully\n", filePath);
    return true;
}

void s3Model::draw()
{
    for (auto m : meshes)
        m->draw();
}

void s3Model::setObjectToWorld(t3Matrix4x4 objectToWorld)
{
    this->objectToWorld = objectToWorld;
}

void s3Model::setWoldToObject(t3Matrix4x4 worldToObject)
{
    this->worldToObject = worldToObject;
}

t3Matrix4x4 s3Model::getObjectToWorld() const
{
    return objectToWorld;
}

t3Matrix4x4 s3Model::getWoldToObject() const
{
    return worldToObject;
}

std::vector<s3Mesh*>& s3Model::getMeshs()
{
    return meshes;
}

void s3Model::clear()
{
    for (auto m : meshes)
        S3_SAFE_DELETE(m);
    meshes.clear();
}
