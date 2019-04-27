#include <directx/texture/s3Texture.h>
#include <core/log/s3Log.h>
#include <app/s3Renderer.h>

s3Texture::s3Texture() :
	created(false),
	wrapMode(S3_TEXTURE_WRAPMODE_CLAMP),
	filterMode(S3_TEXTURE_FILTERMODE_BILINEAR),
	dimension(S3_TEXTURE_DIMENSION_TEXTURE2D),
	format(S3_TEXTURE_FORMAT_R32G32B32A32_UINT),
	width(0),
	height(0),
	mipLevels(1),
	name(""),
	texture2d(nullptr),
	srv(nullptr)
{
}

s3Texture::~s3Texture()
{
	if (isCreated())
	{
		S3_SAFE_RELEASE(srv);
		S3_SAFE_RELEASE(texture2d);
	}
}

bool s3Texture::create()
{
	if (!check()) return false;

	s3Renderer& renderer = s3Renderer::get();
	ID3D11Device* device = renderer.getDevice();

	// create texture2d from user-defined configs
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.ArraySize      = 1;
	tex2dDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
	tex2dDesc.CPUAccessFlags = 0;
	tex2dDesc.Format         = (DXGI_FORMAT) format;
	tex2dDesc.Height         = height;
	tex2dDesc.Width          = width;
	tex2dDesc.MipLevels      = mipLevels;
	tex2dDesc.MiscFlags      = 0;
	tex2dDesc.Usage          = D3D11_USAGE_DEFAULT;
	if (renderer.isMSAAEnabled())
	{
		tex2dDesc.SampleDesc.Quality = renderer.getMSAAQuality();
		tex2dDesc.SampleDesc.Count   = renderer.getMSAACount();
	}
	else
	{
		tex2dDesc.SampleDesc.Quality = 0;
		tex2dDesc.SampleDesc.Count   = 1;
	}

	if (FAILED(device->CreateTexture2D(&tex2dDesc, 0, &texture2d)))
		return false;

	// Creating a view of the texture to be used when binding it on a shader to sample
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                    = (DXGI_FORMAT) format;
	srvDesc.ViewDimension             = (D3D_SRV_DIMENSION) dimension;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels       = mipLevels;

	if (FAILED(device->CreateShaderResourceView(texture2d, &srvDesc, &srv)))
		return false;

	return true;
}

bool s3Texture::isCreated() const
{
	return created;
}

bool s3Texture::generateMips()
{
	s3Renderer& renderer = s3Renderer::get();
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	if (srv)
	{
		deviceContext->GenerateMips(srv);
		return true;
	}
	else
	{
		s3Log::warning("s3Texture::generateMips() srv is null");
		return false;
	}
}

ID3D11Texture2D* s3Texture::getTexture2D() const
{
	return texture2d;
}

ID3D11ShaderResourceView* s3Texture::getShaderResourceView() const
{
	return srv;
}

bool s3Texture::check() const
{
	if (width <= 0 ||
		height <= 0 ||
		mipLevels > S3_TEXTURE_MAX_MIPLEVEL ||
		mipLevels < 0 ||
		wrapMode >= S3_TEXTURE_WRAPMODE_COUNT ||
		filterMode >= S3_TEXTURE_FILTERMODE_COUNT ||
		dimension >= S3_TEXTURE_DIMENSION_COUNT ||
		format >= S3_TEXTURE_FORMAT_COUNT ||
		wrapMode < 0 ||
		filterMode < 0 ||
		dimension < 0 ||
		format < 0)
	{
		s3Log::warning("s3Texture::check() failed with invalid parameter");
		return false;
	}

	return true;
}
