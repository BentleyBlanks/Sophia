#include <app/s3App.h>
#include <app/s3Renderer.h>
#include <app/s3Window.h>
#include <app/s3CallbackManager.h>
#include <core/s3Settings.h>
#include <core/s3Event.h>

#include <imgui.h>

// testing
#include <core/log/s3Log.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    typedef s3MouseEvent::s3ButtonType s3ButtonType;
    switch(msg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    {
        bool button[3];
        button[s3ButtonType::LEFT] = (wParam & MK_LBUTTON) != 0;
        button[s3ButtonType::MIDDLE] = (wParam & MK_MBUTTON) != 0;
        button[s3ButtonType::RIGHT] = (wParam & MK_RBUTTON) != 0;
        
        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int32 x = ((int)(short)LOWORD(lParam));
        int32 y = ((int)(short)HIWORD(lParam));

        for (int32 i = 0; i < 3; i++)
        {
            if (button[i])
            {
                s3MouseEvent mouseEvent((s3ButtonType)i, x, y, 0, 0, 0, control, shift);

                s3CallbackUserData data;
                data.imageData = &mouseEvent;

                s3CallbackManager::callBack.onMousePressed.trigger(&data);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
        bool button[3];
        button[s3ButtonType::LEFT] = (wParam & MK_LBUTTON) != 0;
        button[s3ButtonType::MIDDLE] = (wParam & MK_MBUTTON) != 0;
        button[s3ButtonType::RIGHT] = (wParam & MK_RBUTTON) != 0;

        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int32 x = ((int)(short)LOWORD(lParam));
        int32 y = ((int)(short)HIWORD(lParam));

        for (int32 i = 0; i < 3; i++)
        {
            if (button[i])
            {
                s3MouseEvent mouseEvent((s3ButtonType)i, x, y, 0, 0, 0, control, shift);

                s3CallbackUserData data;
                data.imageData = &mouseEvent;

                s3CallbackManager::callBack.onMouseReleased.trigger(&data);
            }
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        static bool first;
        static int32 lastX, lastY;

        bool button[3];
        button[s3ButtonType::LEFT] = (wParam & MK_LBUTTON) != 0;
        button[s3ButtonType::MIDDLE] = (wParam & MK_MBUTTON) != 0;
        button[s3ButtonType::RIGHT] = (wParam & MK_RBUTTON) != 0;

        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int32 x = ((int)(short)LOWORD(lParam));
        int32 y = ((int)(short)HIWORD(lParam));

        int32 offsetX = 0, offsetY = 0;
        if (first)
        {
            lastX = x;
            lastY = y;
        }
        else
        {
            offsetX = x - lastX;
            offsetY = y - lastY;

            lastX = x;
            lastY = y;
        }

        for (int32 i = -1; i < 3; i++)
        {
            s3MouseEvent mouseEvent(s3ButtonType::NONE, x, y, offsetX, offsetY, 0, control, shift);

            if (i != -1 && button[i])
                mouseEvent.type = (s3ButtonType) i;

            s3CallbackUserData data;
            data.imageData = &mouseEvent;

            s3CallbackManager::callBack.onMouseMoved.trigger(&data);
        }
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        // The distance the mouse wheel is rotated.
        // A positive value indicates the wheel was rotated to the right.
        // A negative value indicates the wheel was rotated to the left.
        float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
        short keyStates = (short)LOWORD(wParam);

        bool shift = (keyStates & MK_SHIFT) != 0;
        bool control = (keyStates & MK_CONTROL) != 0;

        int32 x = ((int)(short)LOWORD(lParam));
        int32 y = ((int)(short)HIWORD(lParam));

        s3MouseEvent mouseEvent(s3ButtonType::NONE, x, y, 0, 0, zDelta, control, shift);
        s3CallbackUserData data;
        data.imageData = &mouseEvent;

        s3CallbackManager::callBack.onMouseScrolled.trigger(&data);
        return 0;
    }

    case WM_KEYDOWN:
    {
        MSG charMsg;
        // Get the unicode character (UTF-16)
        uint32 c = 0;
        // For printable characters, the next message will be WM_CHAR.
        // This message contains the character code we need to send the KeyPressed event.
        // Inspired by the SDL 1.2 implementation.
        if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
        {
            GetMessage(&charMsg, hwnd, 0, 0);
            c = (uint32) charMsg.wParam;
        }

        bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
        bool control = GetAsyncKeyState(VK_CONTROL) > 0;
        bool alt = GetAsyncKeyState(VK_MENU) > 0;

        s3KeyCode key = (s3KeyCode)wParam;
        s3KeyEvent keyEvent(key, c, control, shift, alt);
        s3CallbackUserData data;
        data.imageData = &keyEvent;
        
        s3CallbackManager::callBack.onKeyPressed.trigger(&data);
        return 0;
    }

    case WM_KEYUP:
    {
        bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
        bool control = GetAsyncKeyState(VK_CONTROL) > 0;
        bool alt = GetAsyncKeyState(VK_MENU) > 0;

        s3KeyCode key = (s3KeyCode)wParam;

        uint32 c = 0;
        uint32 scanCode = (lParam & 0x00FF0000) >> 16;

        // Determine which key was released by converting the key code and the scan code
        // to a printable character (if possible).
        // Inspired by the SDL 1.2 implementation.
        uint8 keyboardState[256];
        GetKeyboardState(keyboardState);
        wchar_t translatedCharacters[4];
        if (int result = ToUnicodeEx((uint32) wParam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
        {
            c = translatedCharacters[0];
        }

        s3KeyEvent keyEvent(key, c, control, shift, alt);
        s3CallbackUserData data;
        data.imageData = &keyEvent;
        s3CallbackManager::callBack.onKeyReleased.trigger(&data);
        return 0;
    }

    case WM_SIZE:
    {
        float32 w = (float32) LOWORD(lParam), h = (float32) HIWORD(lParam);
        if(w != 0.0f && h != 0.0f)
            s3Renderer::get().resize((int32)w, (int32)h);
        return 0;
    }

    // System
    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

s3App::s3App()
{
    window = nullptr;
    clearColor.set(0.0f, 0.0f, 0.0f, 1.0f);
}

s3App::~s3App()
{
    S3_SAFE_DELETE(window);
}

bool s3App::init(const t3Vector2f& size, const t3Vector2f& pos)
{
    s3CallbackInit();

    window = new s3Window("Sophia", windowProc, size, pos);
    if(!window)	
        return false;

    // Window correlation
    RECT clientArea;
    GetClientRect(window->getHandle(), &clientArea);
    int32 width = clientArea.right - clientArea.left;
    int32 height = clientArea.bottom - clientArea.top;

    if(!s3Renderer::get().init(window->getHandle(), width, height))	return false;

    return true;
}

void s3App::shutdown()
{
    S3_SAFE_DELETE(window);
    s3Renderer::get().shutdown();

    s3CallbackDeinit();
}

void s3App::render()
{
    s3CallbackManager::callBack.onUpdate.trigger();

    //s3Renderer::get().clear(t3Vector4f(.75f, .75f, .75f, 1.0f));
    s3Renderer::get().clear(clearColor);

    s3CallbackManager::callBack.onBeginRender.trigger();
    s3CallbackManager::callBack.onEndRender.trigger();

    s3Renderer::get().present(0, 0);
}

void s3App::run()
{
    // init engine with hwnd
    s3CallbackUserData imageData;
    imageData.imageData = window->getHandle();
    s3CallbackManager::callBack.onEngineInit.trigger(&imageData);

    MSG msg;
    while(true)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                shutdown();
                return;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        render();
    }
}

void s3App::setClearColor(t3Vector4f clearColor)
{
    this->clearColor = clearColor;
}

s3Window * s3App::getWindow()
{
    return window;
}
