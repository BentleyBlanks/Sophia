#include <directx/texture/s3RenderTexture.h>
#include <core/log/s3Log.h>
#include <app/s3Renderer.h>

s3RenderTexture::s3RenderTexture():
	s3Texture(),
	depth(0),
	antiAlising(0)
{
}

s3RenderTexture::~s3RenderTexture()
{
	if (isCreated())
	{
		S3_SAFE_RELEASE(renderTargetView);
	}
}

bool s3RenderTexture::create()
{
	if (!check()) return false;

	if (!s3Texture::create()) return false;

	s3Renderer& renderer = s3Renderer::get();
	ID3D11Device* device = renderer.getDevice();

	// texture2d and srv ready
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format             = (DXGI_FORMAT) format;
	renderTargetViewDesc.ViewDimension      = (D3D11_RTV_DIMENSION) dimension;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateRenderTargetView(texture2d, 0, &renderTargetView)))
		return false;

	return true;
}

ID3D11RenderTargetView * s3RenderTexture::getRenderTargetView() const
{
	return renderTargetView;
}

bool s3RenderTexture::check() const
{
	if (!s3Texture::check()) return false;

	if (depth != 0 ||
		depth != 16 ||
		depth != 24 ||
		depth != 32 ||
		antiAlising <= 0)
	{
		s3Log::warning("s3RenderTexture::check() failed with invalid parameter");
		return false;
	}

	return true;
}
