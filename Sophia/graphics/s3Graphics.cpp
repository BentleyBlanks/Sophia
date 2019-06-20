#include <graphics/s3Graphics.h>
#include <graphics/s3Renderer.h>
#include <graphics/s3Material.h>
#include <texture/s3RenderTexture.h>
#include <core/s3Settings.h>
#include <core/log/s3Log.h>
#include <imgui.h>

void s3Graphics::drawTextureOnGui(s3Texture* texture)
{
	ImGui::Image((void*)texture->getShaderResourceView(), ImVec2((float32)texture->width, (float32)texture->height));
}

void s3Graphics::drawMesh(s3Mesh* mesh, t3Matrix4x4 localToWorld, s3Material* material)
{}

void s3Graphics::blit(s3Texture* src, s3Texture* dst, s3Material* material)
{
	if (!dst || !dst->isRenderTarget()) return;

	auto renderTexture = (s3RenderTexture*) dst;
	auto renderer      = &s3Renderer::get();
	auto deviceContext = renderer->getDeviceContext();
	auto rtv           = renderTexture->getRenderTargetView();

	// IA
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(renderer->getRasterizerState());

	// vs / ps
	deviceContext->VSSetShader(material->getVertexShader(), nullptr, 0);
	deviceContext->PSSetShader(material->getPixelShader(), nullptr, 0);

	// shader resources
	if (src)
	{
		auto srv = src->getShaderResourceView();
		auto* samplerState = src->getSamplerState();

		deviceContext->PSSetSamplers(0, 1, &samplerState);
		deviceContext->PSSetShaderResources(0, 1, &srv);
	}

	// multiple constant buffer
	auto bufferList   = material->getConstantBuffer();
	auto dataList     = material->getConstantBufferData();
	auto dataSizeList = material->getConstantBufferDataSize();
	auto cbNum        = material->getConstantBufferNum();

	for (int32 i = 0; i < cbNum; i++)
	{
		// Using Map / Unmap instead of UpdateSubresource
		//deviceContext->UpdateSubresource((*bufferList)[i], 0, nullptr, (*dataList)[i], 0, 0);

		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT result = deviceContext->Map((*bufferList)[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		if (FAILED(result))
		{
			s3Log::error("s3Graphics::blit() Map constant buffer failed\n");
			continue;
		}
		else
		{
			memcpy(ms.pData, (*dataList)[i], (*dataSizeList)[i]);
			deviceContext->Unmap((*bufferList)[i], 0);
		}
	}

	// constant buffer could be null
	if (cbNum > 0)
		deviceContext->PSSetConstantBuffers(0, cbNum, bufferList->data());

	deviceContext->OMSetDepthStencilState(renderer->getDepthStencilState(), 1);
	deviceContext->OMSetRenderTargets(1, &rtv, NULL);
	deviceContext->Draw(3, 0);

	// rt need to be restored to renderer's, incase it will be used as srv
	auto currentColorTexture    = (s3RenderTexture*)renderer->currentColorTexture;
	auto currentDepthTexture    = (s3RenderTexture*)renderer->currentDepthTexture;
	auto currentColorTextureRTV = currentColorTexture->getRenderTargetView();
	auto currentDepthTextureDSV = currentDepthTexture->getDepthStencilView();
	deviceContext->OMSetRenderTargets(1, &currentColorTextureRTV, currentDepthTextureDSV);
}

void s3Graphics::setRenderTarget(s3Texture* color, s3Texture* depth)
{
	if (!color || !color->isRenderTarget()) return;

	auto renderTexture    = (s3RenderTexture*) color;
	auto renderer         = &s3Renderer::get();
	auto deviceContext    = renderer->getDeviceContext();
	auto renderTargetView = renderTexture->getRenderTargetView();

	renderer->currentColorTexture = renderTexture;

	if (depth && depth->isDepthTexture())
	{
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depth->getDepthStencilView());
		renderer->currentDepthTexture = depth;
	}
	else
	{
		deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
	}
}
