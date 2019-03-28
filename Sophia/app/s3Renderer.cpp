#include <app/s3Renderer.h>
#include <t3Vector4.h>
#include <core/log/s3Log.h>

s3Renderer::s3Renderer()
  : device(nullptr),
    deviceContext(nullptr),
    swapChain(nullptr),
    renderTargetView(nullptr),
    depthStencilView(nullptr),
    depthStencilBuffer(nullptr),
    depthStencilState(nullptr),
    rasterizerState(nullptr),
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

bool s3Renderer::init(HWND hwnd, int width, int height)
{
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

    HRESULT  hr = D3D11CreateDevice(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 
        createDeviceFlags, 
        featureLevels, _countof(featureLevels),
        D3D11_SDK_VERSION, 
        &device, 
        &featureLevel, 
        &deviceContext);

    if (FAILED(hr))
    {
        s3Log::error("D3D11CreateDevice Failed.\n");
        return false;
    }

    // ------------------------------------------Swap Chain------------------------------------------
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, MSAACount, &MSAAQuality);

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferCount                        = 1;
    scd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Height                  = height;
    scd.BufferDesc.Width                   = width;
    scd.BufferDesc.RefreshRate.Numerator   = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.Flags                              = 0;
    scd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    scd.OutputWindow                       = hwnd;
    if (MSAAEnabled)
    {
        scd.SampleDesc.Count = MSAACount;
        scd.SampleDesc.Quality = MSAAQuality - 1;
    }
    else
    {
        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;
    }
    scd.Windowed = true;

    IDXGIDevice* dxgiDevice = 0;
    device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    IDXGIAdapter* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

    IDXGIFactory* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    hr = dxgiFactory->CreateSwapChain(device, &scd, &swapChain);

    S3_SAFE_RELEASE(dxgiDevice);
    S3_SAFE_RELEASE(dxgiAdapter);
    S3_SAFE_RELEASE(dxgiFactory);

    if(FAILED(hr))
    {
        s3Log::error("D3D11CreateDevice Failed.\n");
        return false;
    }

    // ------------------------------------------Depth / Stencil Texture------------------------------------------
    // Create render target view for back buffer
    ID3D11Texture2D* backBuffer = NULL;
    if(FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBuffer)))	
        return false;
    if(FAILED(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView)))
        return false;
    S3_SAFE_RELEASE(backBuffer);

    // Depth/Stencil Texture Creation
    D3D11_TEXTURE2D_DESC td;
    td.ArraySize      = 1;
    td.MipLevels      = 1;
    td.Width          = width;
    td.Height         = height;
    td.Format         = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.CPUAccessFlags = 0;
    td.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    td.MiscFlags      = 0;
    if (MSAAEnabled)
    {
        td.SampleDesc.Count = MSAACount;
        td.SampleDesc.Quality = MSAAQuality - 1;
    }
    else
    {
        td.SampleDesc.Count = 1;
        td.SampleDesc.Quality = 0;
    }
    td.Usage = D3D11_USAGE_DEFAULT;

    if(FAILED(device->CreateTexture2D(&td, 0, &depthStencilBuffer)))
        return false;
    if(FAILED(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView)))
        return false;

    // Bind the Views to the Output Merger Stage
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // ------------------------------------------Depth/Stencil State------------------------------------------
    bool MSAAEnabled = s3Renderer::get().getMSAAEnabled();

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
    S3_SAFE_RELEASE(swapChain);
    S3_SAFE_RELEASE(renderTargetView);
    S3_SAFE_RELEASE(depthStencilView);
    S3_SAFE_RELEASE(deviceContext);

#ifdef _DEBUG
    // ref: http://masterkenth.com/directx-leak-debugging/
    // reporting live objects ref count
    ID3D11Debug* debugDevice = nullptr;
    HRESULT hr = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast< void** >(&debugDevice));
    if(hr == S_OK)
    {
        hr = debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        S3_SAFE_RELEASE(debugDevice);
    }
#endif

    S3_SAFE_RELEASE(device);
}

void s3Renderer::resize(int width, int height)
{
    // ref: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075%28v=vs.85%29.aspx#Handling_Window_Resizing
    if(!device || !deviceContext || !swapChain) return;

    S3_SAFE_RELEASE(renderTargetView);
    S3_SAFE_RELEASE(depthStencilView);
    S3_SAFE_RELEASE(depthStencilBuffer);

    deviceContext->OMSetRenderTargets(0, 0, 0);

    HRESULT hr;
    // Resize the swap chain and recreate the render target view.
    hr = swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
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

    hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    if(FAILED(hr))
    {
        s3Log::error("Resize CreateRenderTargetView Failed\n");
        return;
    }
    S3_SAFE_RELEASE(backBuffer);

    // https://gamedev.stackexchange.com/questions/86164/idxgiswapchainresizebuffers-should-i-recreate-the-depth-stencil-buffer-too
    // depth/stencil's size match the window size
    // Depth/Stencil Texture Creation
    D3D11_TEXTURE2D_DESC td;
    td.ArraySize      = 1;
    td.MipLevels      = 1;
    td.Width          = (uint32)width;
    td.Height         = (uint32)height;
    td.Format         = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.CPUAccessFlags = 0;
    td.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    td.MiscFlags      = 0; 
    if (MSAAEnabled)
    {
        td.SampleDesc.Count = MSAACount;
        td.SampleDesc.Quality = MSAAQuality - 1;
    }
    else
    {
        td.SampleDesc.Count = 1;
        td.SampleDesc.Quality = 0;
    }
    td.Usage = D3D11_USAGE_DEFAULT;

    hr = device->CreateTexture2D(&td, 0, &depthStencilBuffer);
    if(FAILED(hr))
        return;
    hr = device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);
    if (FAILED(hr))
        return;

    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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
    const float color[4] = {c[0], c[1], c[2], c[3]};

    deviceContext->ClearRenderTargetView(renderTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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

ID3D11RenderTargetView *& s3Renderer::getRenderTargetView()
{
    return renderTargetView;
}

ID3D11DepthStencilView *& s3Renderer::getDepthStencilView()
{
    return depthStencilView;
}

ID3D11DepthStencilState *& s3Renderer::getDepthStencilState()
{
    return depthStencilState;
}

ID3D11RasterizerState *& s3Renderer::getRasterizerState()
{
    return rasterizerState;
}

void s3Renderer::setMSAACount(int count)
{
    if (count >= 1)
        MSAACount = count;
}

int s3Renderer::getMSAACount()
{
    return MSAACount;
}

void s3Renderer::setMSAAEnabled(bool enabled)
{
    MSAAEnabled = enabled;
}

bool s3Renderer::getMSAAEnabled()
{
    return MSAAEnabled;
}
