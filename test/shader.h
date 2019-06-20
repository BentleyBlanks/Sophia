#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector4.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

s3App* app                = nullptr;
s3Renderer* renderer      = nullptr;
s3Material* noiseMaterial = nullptr;
s3Texture* perlinNoise    = nullptr;

ID3D11Device* device               = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

ID3D11VertexShader* vertexShader = nullptr;
ID3D11PixelShader* pixelShader = nullptr;

void InitTextures()
{
	perlinNoise = new s3RenderTexture();
	perlinNoise->width    = 256;
	perlinNoise->height   = 256;
	perlinNoise->wrapMode = S3_TEXTURE_WRAPMODE_CLAMP;
	perlinNoise->format   = S3_TEXTURE_FORMAT_R32G32B32A32_FLOAT;
	perlinNoise->name     = "Perlin Noise";
	perlinNoise->create();
}

class s3ShaderCallBack : public s3CallbackHandle
{
public:
	void guiRender()
	{
		ImGui::Begin("Cloud");
		{
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			s3Graphics::drawTextureOnGui(perlinNoise);

			ImGui::End();
		}
	}

	void render()
	{
	}

	void onHandle(const s3CallbackUserData* data)
	{
		if (data->sender == &s3CallbackManager::callBack.onKeyReleased)
		{
			s3KeyEvent* keyEvent = (s3KeyEvent*)data->imageData;
			
			// Shader API test
			if (keyEvent->keyCode == s3KeyCode::A)
			{
				s3Shader shaderTest;
				shaderTest.load("../Sophia/shaders/test.shader");
				shaderTest.print();

				shaderTest.setBool("boolTest", true);
				shaderTest.setFloat("floatTest", 2.2f);
				shaderTest.setFloat2("float2Test", t3Vector2f(2.0f, 3.0f));
				shaderTest.setFloat3("float3Test", t3Vector3f(2.0f, 3.0f, 4.0f));
				shaderTest.setFloat4("float4Test", t3Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
				shaderTest.setInt("intTest", 3);
				shaderTest.setInt2("int2Test", t3Vector2i(1, 2));
				shaderTest.setInt3("int3Test", t3Vector3i(2, 3, 4));
				shaderTest.setInt4("int4Test", t3Vector4i(1, 2, 3, 4));
				shaderTest.setMatrix("cameraToWorld", t3Matrix4x4(11.0f, 22.0f, 33.0f, 44.0f,
																  5.0f, 6.0f, 3.0f, 2.0f,
																  90.0f, 34.0f, 1000.0f, 10002.0f,
																  22.0f, 18.0f, 56.0f, 99.0f));

				bool boolTest             = shaderTest.getBool("boolTest");
				float32 floatTest         = shaderTest.getFloat("floatTest");
				t3Vector2f float2Test     = shaderTest.getFloat2("float2Test");
				t3Vector3f float3Test     = shaderTest.getFloat3("float3Test");
				t3Vector4f float4Test     = shaderTest.getFloat4("float4Test");
				int32 intTest             = shaderTest.getInt("intTest");
				t3Vector2i int2Test       = shaderTest.getInt2("int2Test");
				t3Vector3i int3Test       = shaderTest.getInt3("int3Test");
				t3Vector4i int4Test       = shaderTest.getInt4("int4Test");
				t3Matrix4x4 cameraToWorld = shaderTest.getMatrix("cameraToWorld");
			}
			else if (keyEvent->keyCode == s3KeyCode::R)
			{
				S3_SAFE_DELETE(noiseMaterial);
				noiseMaterial = new s3Material("../Sophia/shaders/perlinNoise.shader");
			}
			else if (keyEvent->keyCode == s3KeyCode::T)
			{
				s3Shader::reloadShaderSystem();
			}
			else if (keyEvent->keyCode == s3KeyCode::E)
			{
				if (noiseMaterial->isLoaded())
					noiseMaterial->print();
			}
		}
		else if (data->sender == &s3CallbackManager::callBack.onBeginRender)
		{
			if (noiseMaterial && noiseMaterial->isLoaded())
			{
				noiseMaterial->setFloat("time", app->getTimeElapsed());
				//s3Log::info("Time: %f\n", noiseMaterial->getFloat("time"));
				s3Graphics::blit(nullptr, perlinNoise, noiseMaterial);
			}

			guiRender();
		}
	}
};

int main()
{
	app = new s3App();

	if (!app->init(t3Vector2f(800, 600), t3Vector2f(10, 10))) return 0;

	app->setClearColor(t3Vector4f(0.2f, 0.2f, 0.2f, 1.0f));

	s3Window* window = app->getWindow();
	float32 width  = window->getWindowSize().x;
	float32 height = window->getWindowSize().y;

	renderer      = &s3Renderer::get();
	device        = renderer->getDevice();
	deviceContext = renderer->getDeviceContext();

	s3ShaderCallBack shaderCallBack;
	s3CallbackManager::callBack.onBeginRender += shaderCallBack;
	s3CallbackManager::callBack.onKeyReleased += shaderCallBack;

	InitTextures();

	app->run();

	return 0;
}