#include <texture/s3RenderTexture.h>
#include <graphics/s3Renderer.h>
#include <core/log/s3Log.h>

s3RenderTexture::s3RenderTexture():
	s3Texture(),
	renderTargetView(nullptr)
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

	// ------------------------------------------RenderTargetView------------------------------------------
	// depth texture could not create render target view
	if (!isDepthTexture())
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = getRTFormat();
		renderTargetViewDesc.ViewDimension = getRTVDimension();
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		if (FAILED(device->CreateRenderTargetView(texture2d, &renderTargetViewDesc, &renderTargetView)))
		{
			s3Log::error("s3RenderTexture::create() renderTargetView create failed\n");
			created = false;
			return false;
		}
	}

	created = true;
	return true;
}

void s3RenderTexture::clear(bool clearRT, t3Vector4f clearColor, bool clearDepth)
{
	s3Renderer& renderer = s3Renderer::get();
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	if(clearRT && renderTargetView)
		deviceContext->ClearRenderTargetView(renderTargetView, &clearColor[0]);

	if(clearDepth && depthStencilView)    
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ID3D11RenderTargetView * s3RenderTexture::getRenderTargetView() const
{
	return renderTargetView;
}

bool s3RenderTexture::check() const
{
	if (!s3Texture::check()) return false;

	if (depth == 0 ||
		depth == 16 ||
		depth == 24 ||
		depth == 32)
	{
		return true;
	}

	s3Log::warning("s3RenderTexture::check() failed with invalid parameter\n");
	return false;
}

uint32 s3RenderTexture::getBindFlags() const
{
	uint32 bindFlags = 0;

	switch (depth)
	{
	case 0:
		bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		break;

	case 16:
	case 24:
	case 32:
	    // D3D11_BIND_RENDER_TARGET and DEPTH_STENCIL can't both be used together.
		bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		break;
	}

	return bindFlags;
}

D3D11_RTV_DIMENSION s3RenderTexture::getRTVDimension() const
{
	s3Renderer& renderer = s3Renderer::get();

	switch (dimension)
	{
	case S3_TEXTURE_DIMENSION_BUFFER:
		return D3D11_RTV_DIMENSION_BUFFER;

	case S3_TEXTURE_DIMENSION_TEX1D:
		return D3D11_RTV_DIMENSION_TEXTURE1D;

	case S3_TEXTURE_DIMENSION_TEX1DARRAY:
		return D3D11_RTV_DIMENSION_TEXTURE1DARRAY;

	case S3_TEXTURE_DIMENSION_TEX2D:
		if (isMSAAEnabled())
			return D3D11_RTV_DIMENSION_TEXTURE2DMS;
		else
			return D3D11_RTV_DIMENSION_TEXTURE2D;

	case S3_TEXTURE_DIMENSION_TEX2DARRAY:
		if (isMSAAEnabled())
			return D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		else
			return D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

	case S3_TEXTURE_DIMENSION_TEX3D:
		return D3D11_RTV_DIMENSION_TEXTURE3D;

	case S3_TEXTURE_DIMENSION_CUBE:
	case S3_TEXTURE_DIMENSION_CUBEARRAY:
	case S3_TEXTURE_DIMENSION_UNKNOWN:
	default:
		return D3D11_RTV_DIMENSION_UNKNOWN;
	}
}
