#pragma once

#include <core/s3Settings.h>
#include <t3Vector4.h>

class s3RenderTexture;
class s3Renderer
{
public:
    static s3Renderer& get();

    bool init(HWND hwnd, int32 width, int32 height);
    void shutdown();
    void resize(int32 width, int32 height);
    void clear(const t3Vector4f& color);
    void present(int32 syncInterval = 0, int32 presentFlag = 0);

    // DirectX
    ID3D11Device*& getDevice();
    ID3D11DeviceContext*& getDeviceContext();
    IDXGISwapChain*& getSwapChain();
    ID3D11DepthStencilState*& getDepthStencilState();
    ID3D11RasterizerState*& getRasterizerState();

	s3RenderTexture*& getDepthTexture();
	s3RenderTexture*& getColorTexture();

	void setMSAACount(uint32 count);
	uint32 getMSAACount() const;

private:
    s3Renderer();

    // DirectX
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11RasterizerState* rasterizerState;

	s3RenderTexture* depthTexture;
	s3RenderTexture* colorTexture;
	
	uint32 msaaCount;
};