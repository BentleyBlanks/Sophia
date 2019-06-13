#pragma once
#include <texture/s3Texture.h>
#include <t3Vector4.h>

class s3RenderTexture : public s3Texture
{
public:
	s3RenderTexture();
	virtual ~s3RenderTexture();

	// Actually creates the RenderTexture.
	virtual bool create();

	// is texture used as a render texture
	virtual bool isRenderTarget() const;

	// clear the renderTargetView and depthStencilView
	void clear(bool clearRT, t3Vector4f clearColor, bool clearDepth);

	ID3D11RenderTargetView* getRenderTargetView() const;
	
	D3D11_RTV_DIMENSION getRTVDimension() const;
	
	virtual uint32 getBindFlags() const;

	friend class s3Renderer;

protected:
	virtual bool check() const;

	ID3D11RenderTargetView* renderTargetView;
};