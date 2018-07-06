#pragma once
#include <core/s3Settings.h>
#include <t3Matrix4x4.h>

class s3Mesh;
class s3Model
{
public:
    s3Model();
    ~s3Model();

    // load the model to each mesh
    bool load(const char* fileName);

    void draw();

    void setObjectToWorld(t3Matrix4x4 objectToWorld);
    void setWoldToObject(t3Matrix4x4 worldToObject);

    t3Matrix4x4 getObjectToWorld() const;
    t3Matrix4x4 getWoldToObject() const;

    std::vector<s3Mesh*>& getMeshs();

private:
    void clear();

    std::vector<s3Mesh*> meshes;

    t3Matrix4x4 objectToWorld, worldToObject;
};