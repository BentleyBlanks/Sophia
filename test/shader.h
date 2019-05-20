#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

s3Renderer* renderer = nullptr;

ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

class s3ShaderCallBack : public s3CallbackHandle
{
public:
	void guiRender()
	{
		ImGui::Begin("Cloud");
		{
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::End();
		}
	}

	void onHandle(const s3CallbackUserData* data)
	{
		if (data->sender == &s3CallbackManager::callBack.onKeyReleased)
		{
			s3KeyEvent* keyEvent = (s3KeyEvent*)data->imageData;
			if (keyEvent->keyCode == s3KeyCode::R)
			{
				s3Shader shaderTest;
				shaderTest.load("../Sophia/shaders/test.shader");
				shaderTest.print();

				shaderTest.SetFloat("tanHalfFovX", 2.2f);
				shaderTest.SetFloat("tanHalfFovY", 5.5f);
				shaderTest.SetInt("test1", 3);
				shaderTest.SetVector("test2", t3Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
				shaderTest.SetMatrix("cameraToWorld", t3Matrix4x4(11.0f, 22.0f, 33.0f, 44.0f,
														  5.0f, 6.0f, 3.0f, 2.0f,
														  90.0f, 34.0f, 1000.0f, 10002.0f,
														  22.0f, 18.0f, 56.0f, 99.0f));

				float tanHalfFovX         = shaderTest.GetFloat("tanHalfFovX");
				float tanHalfFovY         = shaderTest.GetFloat("tanHalfFovY");
				float test1               = shaderTest.GetInt("test1");
				t3Matrix4x4 cameraToWorld = shaderTest.GetMatrix("cameraToWorld");
				t3Vector4f test2          = shaderTest.GetVector("test2");
			}
		}
		else if (data->sender == &s3CallbackManager::callBack.onBeginRender)
		{
			guiRender();
		}
	}
};

int main()
{
	s3App app;
	renderer = &s3Renderer::get();

	if (!app.init(t3Vector2f(800, 600), t3Vector2f(10, 10))) return 0;

	app.setClearColor(t3Vector4f(0.1f, 0.1f, 0.1f, 1.0f));

	s3Window* window = app.getWindow();
	float32 width  = window->getWindowSize().x;
	float32 height = window->getWindowSize().y;

	device        = renderer->getDevice();
	deviceContext = renderer->getDeviceContext();

	s3ShaderCallBack shaderCallBack;
	s3CallbackManager::callBack.onBeginRender += shaderCallBack;
	s3CallbackManager::callBack.onKeyReleased += shaderCallBack;

	app.run();

	return 0;
}