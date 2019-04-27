#include <directx/grpahics/s3Renderer.h>
#include <directx/texture/s3RenderTexture.h>
#include <t3Vector4.h>
#include <core/log/s3Log.h>

s3Renderer::s3Renderer()
  : device(nullptr),
    deviceContext(nullptr),
    swapChain(nullptr),
    depthStencilState(nullptr),
    rasterizerState(nullptr),
	depthTexture(nullptr),
	colorTexture(nullptr),
    MSAAEnabled(true),
    MSAACount(4),
    MSAAQuality(0)
{
}

s3Renderer& s3Renderer::get()
{
    static s3Renderer renderer;
    return renderer;
}

bool s3Renderer::init(HWND hwnd, int32 width, int32 height)
{
	// prevent error when call init() muiltiple times 
	shutdown();

    // ------------------------------------------Device------------------------------------------
    uint32 createDeviceFlags = 0;

    // debug mode for console output message
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(nullptr,
								   D3D_DRIVER_TYPE_HARDWARE,
								   nullptr,
								   createDeviceFlags,
								   featureLevels, 
								   _countof(featureLevels),
								   D3D11_SDK_VERSION,
								   &device,
								   &featureLevel,
								   &deviceContext);

    if (FAILED(hr))
    {
        s3Log::error("D3D11CreateDevice Failed.\n");
        return false;
    }

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // ------------------------------------------Swap Chain------------------------------------------
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, MSAACount, &MSAAQuality);

    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferCount                        = 1;
    scDesc.BufferDesc.Format                  = format;
    scDesc.BufferDesc.Height                  = height;
    scDesc.BufferDesc.Width                   = width;
    scDesc.BufferDesc.RefreshRate.Numerator   = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    scDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.Flags                              = 0;
    scDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.OutputWindow                       = hwnd;
    scDesc.Windowed                           = true;
    if (MSAAEnabled)
    {
        scDesc.SampleDesc.Count   = MSAACount;
        scDesc.SampleDesc.Quality = MSAAQuality - 1;
    }
    else
    {
        scDesc.SampleDesc.Count   = 1;
        scDesc.SampleDesc.Quality = 0;
    }

    IDXGIDevice* dxgiDevice = 0;
    device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    IDXGIAdapter* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

    IDXGIFactory* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    hr = dxgiFactory->CreateSwapChain(device, &scDesc, &swapChain);

    S3_SAFE_RELEASE(dxgiDevice);
    S3_SAFE_RELEASE(dxgiAdapter);
    S3_SAFE_RELEASE(dxgiFactory);

    if(FAILED(hr))
    {
        s3Log::error("D3D11CreateDevice dailed.\n");
        return false;
    }

    // ------------------------------------------Color Texture------------------------------------------
    // create colorTexture's texture2d form swapChain's back buffer
    ID3D11Texture2D* backBuffer = NULL;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& backBuffer)))
	{
		s3Log::error("s3Renderer::init() swapChain->GetBuffer() failed\n");
		return false;
	}

	// user-defined texture2d's colorTexture creation
	colorTexture = new s3RenderTexture();
	colorTexture->texture2d  = backBuffer;
	colorTexture->width      = width;
	colorTexture->height     = height;
	colorTexture->depth      = 0;
	colorTexture->dimension  = S3_TEXTURE_DIMENSION_TEX2D;
	colorTexture->format     = (s3TextureFormat) format;
	colorTexture->filterMode = S3_TEXTURE_FILTERMODE_BILINEAR;
	colorTexture->wrapMode   = S3_TEXTURE_WRAPMODE_CLAMP;
	colorTexture->mipLevels  = S3_TEXTURE_MAX_MIPLEVEL;
	colorTexture->name       = "s3RendererColorTexture";
	colorTexture->create();

    // ------------------------------------------Depth / Stencil Texture------------------------------------------
    // depth / stencil texture creation
	depthTexture = new s3RenderTexture();
	depthTexture->width      = width;
	depthTexture->height     = height;
	depthTexture->depth      = 24;
	depthTexture->dimension  = S3_TEXTURE_DIMENSION_TEX2D;
	depthTexture->format     = (s3TextureFormat) DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexture->filterMode = S3_TEXTURE_FILTERMODE_POINT;
	depthTexture->wrapMode   = S3_TEXTURE_WRAPMODE_CLAMP;
	depthTexture->mipLevels  = 1;
	depthTexture->name       = "s3RendererDepthTexture";
	depthTexture->create();

    // Bind the colorTexture and depthTexture to the Output Merger Stage
    deviceContext->OMSetRenderTargets(1, &colorTexture->renderTargetView, depthTexture->depthStencilView);

    // ------------------------------------------Depth/Stencil State------------------------------------------
    bool MSAAEnabled = s3Renderer::get().isMSAAEnabled();

	// DepthStencil state controls how DepthStencil testing is performed by the OM stage.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilStateDesc.DepthEnable    = MSAAEnabled;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilStateDesc.StencilEnable  = FALSE;

    hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create DepthStencil State\n");
        return false;
    }

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.AntialiasedLineEnable = MSAAEnabled;
    rasterizerDesc.CullMode              = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias             = 0;
    rasterizerDesc.DepthBiasClamp        = 0.0f;
    rasterizerDesc.DepthClipEnable       = TRUE;
    rasterizerDesc.FillMode              = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable     = MSAAEnabled;
    rasterizerDesc.ScissorEnable         = FALSE;
    rasterizerDesc.SlopeScaledDepthBias  = 0.0f;

    // Create the rasterizer state object.
    hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Rasterrizer State\n");
        return false;
    }

    // ------------------------------------------Viewport------------------------------------------
    D3D11_VIEWPORT vp;
    vp.Width    = (float) width;
    vp.Height   = (float) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    deviceContext->RSSetViewports(1, &vp);

    return true;
}

void s3Renderer::shutdown()
{
	S3_SAFE_DELETE(depthTexture);
	S3_SAFE_DELETE(colorTexture);

	S3_SAFE_RELEASE(depthStencilState);
	S3_SAFE_RELEASE(rasterizerState);

#ifdef _DEBUG
    // ref: http://masterkenth.com/directx-leak-debugging/
    // reporting live objects ref count
    ID3D11Debug* debugDevice = nullptr;
	if (device)
	{
		HRESULT hr = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));
		if (hr == S_OK)
		{
			hr = debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			S3_SAFE_RELEASE(debugDevice);
		}
	}
#endif

	S3_SAFE_RELEASE(swapChain);
	S3_SAFE_RELEASE(deviceContext);
    S3_SAFE_RELEASE(device);
}

void s3Renderer::resize(int32 width, int32 height)
{
    // ref: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075%28v=vs.85%29.aspx#Handling_Window_Resizing
    if(!device || !deviceContext || !swapChain) return;

	S3_SAFE_DELETE(depthTexture);
	S3_SAFE_DELETE(colorTexture);

    deviceContext->OMSetRenderTargets(0, 0, 0);

    HRESULT hr;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Resize the swap chain and recreate the render target view.
    hr = swapChain->ResizeBuffers(1, width, height, format, 0);
    if(FAILED(hr))
    {
        s3Log::error("Resize Buffer Failed\n");
        return;
    }

    // Get buffer and create a render-target-view.
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);
    if(FAILED(hr))
    {
        s3Log::error("Resize Get Buffer Failed\n");
        return;
    }

	// user-defined texture2d's colorTexture creation
	colorTexture = new s3RenderTexture();
	colorTexture->texture2d  = backBuffer;
	colorTexture->width      = width;
	colorTexture->height     = height;
	colorTexture->depth      = 0;
	colorTexture->dimension  = S3_TEXTURE_DIMENSION_TEX2D;
	colorTexture->format     = (s3TextureFormat) format;
	colorTexture->filterMode = S3_TEXTURE_FILTERMODE_BILINEAR;
	colorTexture->wrapMode   = S3_TEXTURE_WRAPMODE_CLAMP;
	colorTexture->mipLevels  = S3_TEXTURE_MAX_MIPLEVEL;
	colorTexture->name       = "s3RendererColorTexture";
	colorTexture->create();

    // https://gamedev.stackexchange.com/questions/86164/idxgiswapchainresizebuffers-should-i-recreate-the-depth-stencil-buffer-too
    // depth/stencil's size match the window size
    // Depth/Stencil Texture Creation
	depthTexture = new s3RenderTexture();
	depthTexture->width = width;
	depthTexture->height = height;
	depthTexture->depth = 24;
	depthTexture->dimension = S3_TEXTURE_DIMENSION_TEX2D;
	depthTexture->format = (s3TextureFormat)DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexture->filterMode = S3_TEXTURE_FILTERMODE_POINT;
	depthTexture->wrapMode = S3_TEXTURE_WRAPMODE_CLAMP;
	depthTexture->mipLevels = 1;
	depthTexture->name = "s3RendererDepthTexture";
	depthTexture->create();

    deviceContext->OMSetRenderTargets(1, &colorTexture->renderTargetView, depthTexture->depthStencilView);

    // Set up the viewport.
    D3D11_VIEWPORT vp;
    vp.Width    = (float32) width;
    vp.Height   = (float32) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    deviceContext->RSSetViewports(1, &vp);
}

void s3Renderer::clear(const t3Vector4f& c)
{
	if(colorTexture)
		colorTexture->clear(true, c, false);
	
	if(depthTexture)
		depthTexture->clear(true, c, true);
}

void s3Renderer::present(int32 syncInterval, int32 presentFlag)
{
    if(swapChain)
        swapChain->Present(syncInterval, presentFlag);
}

ID3D11Device*& s3Renderer::getDevice()
{
    return device;
}

ID3D11DeviceContext*& s3Renderer::getDeviceContext()
{
    return deviceContext;
}

IDXGISwapChain *& s3Renderer::getSwapChain()
{
    return swapChain;
}

ID3D11DepthStencilState *& s3Renderer::getDepthStencilState()
{
    return depthStencilState;
}

ID3D11RasterizerState *& s3Renderer::getRasterizerState()
{
    return rasterizerState;
}

s3RenderTexture*& s3Renderer::getDepthTexture()
{
	return depthTexture;
}

s3RenderTexture*& s3Renderer::getColorTexture()
{
	return colorTexture;
}

void s3Renderer::setMSAACount(int32 count)
{
    if (count >= 1)
        MSAACount = count;
}

int32 s3Renderer::getMSAACount()
{
    return MSAACount;
}

void s3Renderer::setMSAAEnabled(bool enabled)
{
    MSAAEnabled = enabled;
}

bool s3Renderer::isMSAAEnabled()
{
    return MSAAEnabled;
}

uint32 s3Renderer::getMSAAQuality() const
{
	return MSAAQuality;
}
