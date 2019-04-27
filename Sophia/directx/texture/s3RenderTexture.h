#pragma once
#include <directx/texture/s3TextureMode.h>

class s3RenderTexture
{
public:
	s3RenderTexture(int width, int height, int depth, s3TextureFormat format);
	virtual ~s3RenderTexture();

	// Actually creates the RenderTexture.
	void create();

	// Is the render texture actually created?
	bool isCreated();

	// Manually re-generate mipmap levels of a render texture.
	void generateMips();

private:
	// The precision of the render texture's depth buffer in bits (0, 16, 24/32 are supported).
	int depth;
	// Anti-aliasing value indicates the number of samples per pixel
	int antiAlising;
	// Render texture has mipmaps when this flag is set / Mipmaps generated automatically
	bool autoGenerateMips, useMipMap;
	// Is the render texture actually created?
	bool created;
};