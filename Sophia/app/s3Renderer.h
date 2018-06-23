#pragma once

#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3Renderer
{
public:
    static s3Renderer& get();

    bool init(HWND hwnd, int width, int height);
    void shutdown();
    void resize(int width, int height);
    void clear(const t3Vector4f& color);
    void present(int32 syncInterval = 0, int32 presentFlag = 0);

    // DirectX
    ID3D11Device*& getDevice();
    ID3D11DeviceContext*& getDeviceContext();
    IDXGISwapChain*& getSwapChain();
    ID3D11RenderTargetView*& getRenderTargetView();
    ID3D11DepthStencilView*& getDepthStencilView();
    ID3D11DepthStencilState*& getDepthStencilState();
    ID3D11RasterizerState*& getRasterizerState();

    void setMSAACount(int count);
    int getMSAACount();
    void setMSAAEnabled(bool enabled);
    bool getMSAAEnabled();

private:
    s3Renderer();

    // DirectX
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;

    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthStencilBuffer;

    ID3D11DepthStencilState* depthStencilState;
    ID3D11RasterizerState* rasterizerState;

    bool MSAAEnabled;
    int32 MSAACount;
    uint32 MSAAQuality;
};