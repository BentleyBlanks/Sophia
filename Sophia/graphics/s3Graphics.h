#pragma once
#include <t3Matrix4x4.h>

class s3Mesh;
class s3Material;
class s3Texture;
class s3Graphics
{
public:
	static void drawTextureOnGui(s3Texture* texture);

	static void drawMesh(s3Mesh* mesh, t3Matrix4x4 localToWorld, s3Material* material);

	// Blit would set dst texture as renderTarget
	static void blit(s3Texture* src, s3Texture* dst, s3Material* material);

	static void setRenderTarget(s3Texture* color, s3Texture* depth = nullptr);
};