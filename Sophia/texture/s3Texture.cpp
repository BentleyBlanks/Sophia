#include <texture/s3Texture.h>
#include <graphics/s3Renderer.h>
#include <core/log/s3Log.h>

s3Texture::s3Texture() :
	created(false),
	wrapMode(S3_TEXTURE_WRAPMODE_CLAMP),
	filterMode(S3_TEXTURE_FILTERMODE_LINEAR),
	dimension(S3_TEXTURE_DIMENSION_TEX2D),
	format(S3_TEXTURE_FORMAT_R32G32B32A32_UINT),
	width(0),
	height(0),
	depth(0),
	mipLevels(1),
	msaaCount(4),
	anisoLevel(1),
	name(""),
	texture2d(nullptr),
	shaderResourceView(nullptr),
	depthStencilView(nullptr)
{
}

s3Texture::~s3Texture()
{
	if (isCreated())
	{
		S3_SAFE_RELEASE(shaderResourceView);
		S3_SAFE_RELEASE(texture2d);
		S3_SAFE_RELEASE(depthStencilView);
	}

	created = false;
}

bool s3Texture::create()
{
	if (!check()) return false;

	s3Renderer& renderer = s3Renderer::get();
	ID3D11Device* device = renderer.getDevice();

	uint32 msaaQuality;
	bool msaaEnabled;
	checkMSAA(&msaaEnabled, &msaaQuality);

	// ========================================== Texture2D ==========================================
	// texture2d already been initialized by other module
	if (!texture2d)
	{
		// create texture2d from user-defined configs
		D3D11_TEXTURE2D_DESC tex2dDesc;
		ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
		tex2dDesc.ArraySize          = 1;
		tex2dDesc.BindFlags          = getBindFlags();
		tex2dDesc.CPUAccessFlags     = 0;
		tex2dDesc.Format             = getTexture2dFormat();
		tex2dDesc.Height             = height;
		tex2dDesc.Width              = width;
		tex2dDesc.MipLevels          = mipLevels;
		tex2dDesc.MiscFlags          = 0;
		tex2dDesc.Usage              = D3D11_USAGE_DEFAULT;
		if (msaaEnabled)
		{
			tex2dDesc.SampleDesc.Quality = msaaQuality - 1;
			tex2dDesc.SampleDesc.Count   = msaaCount;
		}
		else
		{
			tex2dDesc.SampleDesc.Quality = 0;
			tex2dDesc.SampleDesc.Count   = 1;
		}

		if (FAILED(device->CreateTexture2D(&tex2dDesc, 0, &texture2d)))
		{
			s3Log::error("s3Texture::create() create Texture2D failed\n");
			created = false;
			return false;
		}

		// Set debug name of the texture
		texture2d->SetPrivateData(WKPDID_D3DDebugObjectName, (uint32) name.size(), name.c_str());
	}

	// ========================================== ShaderResourceView ==========================================
	// Creating a shaderResourceView of the texture to be used when binding it on a shader to sample
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format                    = getSRVFormat();
	srvDesc.ViewDimension             = getSRVDimension();
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels       = mipLevels;

	if (FAILED(device->CreateShaderResourceView(texture2d, &srvDesc, &shaderResourceView)))
	{
		s3Log::error("s3Texture::create() create ShaderResourceView failed\n");
		created = false;
		return false;
	}

	// ========================================== DepthStencilView ==========================================
	// Creating a depthStencilView of the texture to be used as a depth texture
	if (isDepthTexture())
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		//--! Might be sync with depthDimension
		depthDesc.Texture2D.MipSlice = 0;
		depthDesc.ViewDimension      = getDepthDimension();
		depthDesc.Format             = getDepthFormat();

		if (FAILED(device->CreateDepthStencilView(texture2d, &depthDesc, &depthStencilView)))
		{
			s3Log::error("s3Texture::create() create DepthStencilView failed\n");
			created = false;
			return false;
		}
	}

	// ========================================== SamplerState ==========================================
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter         = getFilterMode();
	samplerDesc.AddressU       = getWrapMode();
	samplerDesc.AddressV       = getWrapMode();
	samplerDesc.AddressW       = getWrapMode();
	samplerDesc.MipLODBias     = 0.0f;
	samplerDesc.MaxAnisotropy  = anisoLevel;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD         = -FLT_MAX;
	samplerDesc.MaxLOD         = FLT_MAX;

	if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState)))
	{
		s3Log::error("s3Texture::create() create SamplerState failed\n");
		created = false;
		return false;
	}

	created = true;
	return true;
}

void s3Texture::clear()
{
	s3Renderer& renderer = s3Renderer::get();
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	if(depthStencilView)
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

bool s3Texture::isCreated() const
{
	return created;
}

bool s3Texture::isMSAAEnabled() const
{
	bool msaaEnabled;
	uint32 msaaQuality;
	checkMSAA(&msaaEnabled, &msaaQuality);

	return msaaEnabled;
}

bool s3Texture::isDepthTexture() const
{
	return depth > 0;
}

bool s3Texture::isRenderTarget() const
{
	return false;
}

bool s3Texture::generateMips()
{
	s3Renderer& renderer = s3Renderer::get();
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	if (shaderResourceView)
	{
		deviceContext->GenerateMips(shaderResourceView);
		return true;
	}
	else
	{
		s3Log::warning("s3Texture::generateMips() srv is null\n");
		return false;
	}
}

ID3D11Texture2D* s3Texture::getTexture2D() const
{
	return texture2d;
}

ID3D11ShaderResourceView* s3Texture::getShaderResourceView() const
{
	return shaderResourceView;
}

ID3D11DepthStencilView * s3Texture::getDepthStencilView() const
{
	return depthStencilView;
}

ID3D11SamplerState* s3Texture::getSamplerState() const
{
	return samplerState;
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
		s3Log::warning("s3Texture::check() failed with invalid parameter\n");
		return false;
	}

	return true;
}

void s3Texture::checkMSAA(bool* msaaEnabled, uint32* msaaQuality) const
{
	s3Renderer& renderer = s3Renderer::get();
	ID3D11Device* device = renderer.getDevice();

	*msaaEnabled = true;
	if (FAILED(device->CheckMultisampleQualityLevels(getDepthFormat(), msaaCount, msaaQuality)))
		*msaaEnabled = false;

	if (*msaaQuality <= 1)
		*msaaEnabled = false;
}

uint32 s3Texture::getBindFlags() const
{
	uint32 bindFlags = D3D11_BIND_SHADER_RESOURCE;

	switch (depth)
	{
	case 0:
		// No depthStencilView created
		break;

	case 16:
	case 24:
	case 32:
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;
		break;
	}

	return bindFlags;
}

DXGI_FORMAT s3Texture::getTexture2dFormat() const
{
	switch (depth)
	{
	case 0:
		return (DXGI_FORMAT)format;

	case 16:
		// A two-component, 32-bit typeless format that supports 16 bits for the red channel and 16 bits for the green channel.
		return DXGI_FORMAT_R16_TYPELESS;

	case 24:
		// A two - component, 32 - bit typeless format that supports 24 bits for the red channel and 8 bits for the green channel.
		return DXGI_FORMAT_R24G8_TYPELESS;

	case 32:
		// 	A two-component, 64-bit typeless format that supports 32 bits for the red channel, 8 bits for the green channel, and 24 bits are unused.
		return DXGI_FORMAT_R32G8X24_TYPELESS;

	default:
		return (DXGI_FORMAT)format;
	}
}

DXGI_FORMAT s3Texture::getDepthFormat() const
{
	switch (depth)
	{
	case 0:
		return (DXGI_FORMAT) format;

	case 16:
		// A single-component, 16-bit unsigned-normalized-integer format that supports 16 bits for depth.
		return DXGI_FORMAT_D16_UNORM;

	case 24:
		// A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	case 32:
		// A 32-bit floating-point component, and two unsigned-integer components (with an additional 32 bits). This format supports 32-bit depth, 8-bit stencil, and 24 bits are unused.
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	default:
		return (DXGI_FORMAT) format;
	}
}

DXGI_FORMAT s3Texture::getRTFormat() const
{
	return getSRVFormat();
}

DXGI_FORMAT s3Texture::getSRVFormat() const
{
	switch (depth)
	{
	case 0:
		return (DXGI_FORMAT)format;

	case 16:
		// A two-component, 32-bit typeless format that supports 16 bits for the red channel and 16 bits for the green channel.
		return DXGI_FORMAT_R16_FLOAT;

	case 24:
		// A two - component, 32 - bit typeless format that supports 24 bits for the red channel and 8 bits for the green channel.
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	case 32:
		// 	A two-component, 64-bit typeless format that supports 32 bits for the red channel, 8 bits for the green channel, and 24 bits are unused.
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

	default:
		return (DXGI_FORMAT)format;
	}
}

D3D11_TEXTURE_ADDRESS_MODE s3Texture::getWrapMode() const
{
	// D3D11_TEXTURE_ADDRESS_BORDER not supported
	switch (wrapMode)
	{
	case S3_TEXTURE_WRAPMODE_REPEAT:
		return D3D11_TEXTURE_ADDRESS_WRAP;

	case S3_TEXTURE_WRAPMODE_CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;

	case S3_TEXTURE_WRAPMODE_MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;

	case S3_TEXTURE_WRAPMODE_MIRRORONCE:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;

	default:
		s3Log::warning("s3Texture::getWrapMode() wrapMode %d not supported\n", (int32) wrapMode);
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}

D3D11_FILTER s3Texture::getFilterMode() const
{
	switch (filterMode)
	{
	case S3_TEXTURE_FILTERMODE_POINT:
		return D3D11_FILTER_MIN_MAG_MIP_POINT;

	case S3_TEXTURE_FILTERMODE_LINEAR:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	case S3_TEXTURE_FILTERMODE_ANISOTROPIC:
		return D3D11_FILTER_ANISOTROPIC;

	default:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
}

D3D11_SRV_DIMENSION s3Texture::getSRVDimension() const
{
	s3Renderer& renderer = s3Renderer::get();

	switch (dimension)
	{
	case S3_TEXTURE_DIMENSION_BUFFER:
		return D3D_SRV_DIMENSION_BUFFER;

	case S3_TEXTURE_DIMENSION_TEX1D:
		return D3D_SRV_DIMENSION_TEXTURE1D;

	case S3_TEXTURE_DIMENSION_TEX1DARRAY:
		return D3D_SRV_DIMENSION_TEXTURE1DARRAY;

	case S3_TEXTURE_DIMENSION_TEX2D:
		if (isMSAAEnabled())
			return D3D_SRV_DIMENSION_TEXTURE2DMS;
		else
			return D3D_SRV_DIMENSION_TEXTURE2D;

	case S3_TEXTURE_DIMENSION_TEX2DARRAY:
		if (isMSAAEnabled())
			return D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
		else
			return D3D_SRV_DIMENSION_TEXTURE2DARRAY;

	case S3_TEXTURE_DIMENSION_TEX3D:
		return D3D_SRV_DIMENSION_TEXTURE3D;

	case S3_TEXTURE_DIMENSION_CUBE:
		return D3D_SRV_DIMENSION_TEXTURECUBE;

	case S3_TEXTURE_DIMENSION_CUBEARRAY:
		return D3D_SRV_DIMENSION_TEXTURECUBEARRAY;

	case S3_TEXTURE_DIMENSION_UNKNOWN:
	default:
		return D3D_SRV_DIMENSION_UNKNOWN;
	}
}

D3D11_DSV_DIMENSION s3Texture::getDepthDimension() const
{
	s3Renderer& renderer = s3Renderer::get();
	
	switch (dimension)
	{
	case S3_TEXTURE_DIMENSION_TEX1D:
		return D3D11_DSV_DIMENSION_TEXTURE1D;

	case S3_TEXTURE_DIMENSION_TEX1DARRAY:
		return D3D11_DSV_DIMENSION_TEXTURE1DARRAY;

	case S3_TEXTURE_DIMENSION_TEX2D:
		if (isMSAAEnabled())
			return D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			return D3D11_DSV_DIMENSION_TEXTURE2D;

	case S3_TEXTURE_DIMENSION_TEX2DARRAY:
		if (isMSAAEnabled())
			return D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		else
			return D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

	case S3_TEXTURE_DIMENSION_BUFFER:
	case S3_TEXTURE_DIMENSION_TEX3D:
	case S3_TEXTURE_DIMENSION_CUBE:
	case S3_TEXTURE_DIMENSION_CUBEARRAY:
	case S3_TEXTURE_DIMENSION_UNKNOWN:
	default:
		return D3D11_DSV_DIMENSION_UNKNOWN;
	}
}
