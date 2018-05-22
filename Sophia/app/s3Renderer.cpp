#include <app/s3Renderer.h>
#include <t3Vector4.h>
#include <core/log/s3Log.h>

s3Renderer::s3Renderer()
  : device(NULL),
    deviceContext(NULL),
    swapChain(NULL),
    renderTargetView(NULL),
    depthStencilView(NULL)
{
}

s3Renderer& s3Renderer::get()
{
    static s3Renderer renderer;
    return renderer;
}

bool s3Renderer::init(HWND hwnd, int width, int height)
{
    // ------------------------Device And SwapChain------------------------
    uint32 createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevelSupported;

    // debug mode for console output message
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevelsRequested[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Swap Chain No MSAA 
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.Flags = 0;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = true;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        featureLevelsRequested, 
        _countof(featureLevelsRequested),
        D3D11_SDK_VERSION,
        &scd,
        &swapChain,
        &device,
        &featureLevelSupported,
        &deviceContext);

    if(FAILED(hr))
    {
        s3Log::error("D3D11CreateDevice Failed.\n");
        return false;
    }

    // ------------------------Depth/Stencil------------------------
    // Create render target view for back buffer
    ID3D11Texture2D* backBuffer = NULL;
    if(FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBuffer)))	
        return false;
    if(FAILED(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView)))
        return false;
    backBuffer->Release();

    // Depth/Stencil Texture Creation
    D3D11_TEXTURE2D_DESC td;
    td.ArraySize = 1;
    td.MipLevels = 1;
    td.Width = width;
    td.Height = height;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.CPUAccessFlags = 0;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.MiscFlags = 0;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;

    ID3D11Texture2D* depthStencilBuffer = NULL;
    if(FAILED(device->CreateTexture2D(&td, 0, &depthStencilBuffer)))
        return false;
    if(FAILED(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView)))
        return false;
    depthStencilBuffer->Release();

    // Bind the Views to the Output Merger Stage
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    D3D11_VIEWPORT vp;
    vp.Width = (float) width;
    vp.Height = (float) height;
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
    if(!deviceContext || !swapChain)
        return;

    deviceContext->OMSetRenderTargets(0, 0, 0);

    if(renderTargetView)
        renderTargetView->Release();

    HRESULT hr;
    // Preserve the existing buffer count and format.
    // Automatically choose the width and height to match the client rect for HWNDs.
    hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(hr))
    {
        s3Log::error("Resize Buffer Failed\n");
        return;
    }

    // Get buffer and create a render-target-view.
    ID3D11Texture2D* pBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &pBuffer);
    if(FAILED(hr))
    {
        s3Log::error("Resize Get Buffer Failed\n");
        return;
    }

    hr = device->CreateRenderTargetView(pBuffer, NULL, &renderTargetView);
    if(FAILED(hr))
    {
        s3Log::error("Resize CreateRenderTargetView Failed\n");
        return;
    }
    pBuffer->Release();

    // https://gamedev.stackexchange.com/questions/86164/idxgiswapchainresizebuffers-should-i-recreate-the-depth-stencil-buffer-too
    // depth/stencil's size match the window size
    if(depthStencilView)	
        depthStencilView->Release();

    // Depth/Stencil Texture Creation
    D3D11_TEXTURE2D_DESC td;
    td.ArraySize = 1;
    td.MipLevels = 1;
    td.Width = width;
    td.Height = height;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.CPUAccessFlags = 0;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.MiscFlags = 0;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;

    ID3D11Texture2D* depthStencilBuffer = NULL;
    if(FAILED(device->CreateTexture2D(&td, 0, &depthStencilBuffer)))
        return;
    if(FAILED(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView)))
        return;
    depthStencilBuffer->Release();

    deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

    // Set up the viewport.
    D3D11_VIEWPORT vp;
    vp.Width = (float32) width;
    vp.Height = (float32) height;
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

ID3D11Device* s3Renderer::getDevice()
{
    return device;
}

ID3D11DeviceContext* s3Renderer::getDeviceContext()
{
    return deviceContext;
}

IDXGISwapChain * s3Renderer::getSwapChain()
{
    return swapChain;
}

ID3D11RenderTargetView * s3Renderer::getRenderTargetView()
{
    return renderTargetView;
}

ID3D11DepthStencilView * s3Renderer::getDepthStencilView()
{
    return depthStencilView;
}