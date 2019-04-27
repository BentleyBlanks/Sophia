#pragma once
#include <directx/texture/s3Texture.h>

class s3RenderTexture : public s3Texture
{
public:
	s3RenderTexture();
	virtual ~s3RenderTexture();

	// Actually creates the RenderTexture.
	virtual bool create();

	// The precision of the render texture's depth buffer in bits (0, 16, 24/32 are supported).
	int32 depth;
	// Anti-aliasing value indicates the number of samples per pixel
	int32 antiAlising;

	ID3D11RenderTargetView* getRenderTargetView() const;

protected:
	virtual bool check() const;

	ID3D11RenderTargetView* renderTargetView;
};