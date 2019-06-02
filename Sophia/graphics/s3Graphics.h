#pragma once
#include <t3Matrix4x4.h>

class s3Mesh;
class s3Material;
class s3Texture;
class s3Graphics
{
public:
	static void DrawTextureOnGui(s3Texture* texture);

	static void DrawMesh(s3Mesh* mesh, t3Matrix4x4 localToWorld, s3Material* material);

	static void Blit(s3Texture* src, s3Texture* dst, s3Material* material);

	static void SetRenderTarget(s3Texture* rt);
};