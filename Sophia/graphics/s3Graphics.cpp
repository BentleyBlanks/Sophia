#include <graphics/s3Graphics.h>
#include <graphics/s3Renderer.h>
#include <graphics/s3Material.h>
#include <texture/s3RenderTexture.h>

void s3Graphics::DrawTextureOnGui(s3Texture* texture)
{}

void s3Graphics::DrawMesh(s3Mesh* mesh, t3Matrix4x4 localToWorld, s3Material* material)
{}

void s3Graphics::Blit(s3Texture* src, s3Texture* dst, s3Material* material)
{
	auto renderer         = &s3Renderer::get();
	auto deviceContext    = renderer->getDeviceContext();
	auto renderTargetView = renderer->getColorTexture()->getRenderTargetView();

	// IA
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(renderer->getRasterizerState());

	// vs
	deviceContext->VSSetShader(material->GetVertexShader(), nullptr, 0);

	// ps
	auto srv = src->getShaderResourceView();
	auto* samplerState = src->getSamplerState();

	deviceContext->PSSetShader(material->GetPixelShader(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &samplerState);
	deviceContext->PSSetShaderResources(0, 1, &srv);

	// multiple constant buffer
	for (int32 i = 0; i < material->GetConstantBufferNum(); i++)
	{
		auto bufferList = material->GetConstantBuffer();
		auto dataList   = material->GetConstantBufferData();
		deviceContext->UpdateSubresource((*bufferList)[i], 0, nullptr, &dataList[i], 0, 0);
	}
	deviceContext->PSSetConstantBuffers(0, material->GetConstantBufferNum(), bufferList->data());

	deviceContext->OMSetDepthStencilState(renderer->getDepthStencilState(), 1);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, renderer->getDepthTexture()->getDepthStencilView());
	deviceContext->Draw(3, 0);
}

void s3Graphics::SetRenderTarget(s3Texture* rt)
{
	auto renderer         = &s3Renderer::get();
	auto deviceContext    = renderer->getDeviceContext();
	auto renderTargetView = renderer->getColorTexture()->getRenderTargetView();

	deviceContext->OMSetRenderTargets(1, &renderTargetView, renderer->getDepthTexture()->getDepthStencilView());
}
