#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ImDrawData;

bool s3ImGuiInit(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
void s3ImGuiShutdown();
void s3ImGuiBeginRender();
void s3ImGuiEndRender();
