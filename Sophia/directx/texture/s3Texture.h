#pragma once
#include <core/s3Settings.h>
#include <directx/texture/s3TextureMode.h>

class s3Texture
{
public:
	s3Texture();
	virtual ~s3Texture();

	// actually creates the RenderTexture.
	virtual bool create();

	// clear the depthStencilView
	void clear();

	// is the texture actually created?
	bool isCreated() const;

	// is MSAA supported according to MSAACount and texture format
	bool isMSAAEnabled() const;

	// is texture used as a depth texture
	bool isDepthTexture() const;

	// Manually re-generate mipmap levels of a render texture.
	bool generateMips();

	// Texture coordinate wrapping mode.
	s3TextureWrapMode wrapMode;

	// Filtering mode of the texture.
	s3TextureFilterMode filterMode;

	// Dimensionality (type) of the texture.
	s3TextureDimension dimension;

	// texture's srv / rt format of a texture object.(depth format determined by the depth)
	s3TextureFormat format;

	// size of the texture
	int32 width, height;

	// The precision of the texture's depth buffer in bits (0, 16, 24, 32 are supported).
	int32 depth;

	// The maximum number of mipmap levels in the texture. 1: multisampled texture, 0: generate a full set of subtextures.
	int32 mipLevels;

	// The number of samples during multisampling.(no more than D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT: 32)
	int32 msaaCount;

	// name of the texture
	std::string name;

	ID3D11Texture2D* getTexture2D() const;
	ID3D11ShaderResourceView* getShaderResourceView() const;
	ID3D11DepthStencilView* getDepthStencilView() const;
	
	DXGI_FORMAT getTexture2dFormat() const;
	DXGI_FORMAT getDepthFormat() const;
	DXGI_FORMAT getRTFormat() const;
	DXGI_FORMAT getSRVFormat() const;

	D3D11_SRV_DIMENSION getSRVDimension() const;
	D3D11_DSV_DIMENSION getDepthDimension() const;

	virtual uint32 getBindFlags() const;

	friend class s3Renderer;

protected:
	bool check() const;

	// check support of MSAA according to MSAACount and texture format
	void checkMSAA(bool* msaaEnabled, uint32* msaaQuality) const;

	bool created;

	ID3D11Texture2D* texture2d;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11DepthStencilView* depthStencilView;
};
