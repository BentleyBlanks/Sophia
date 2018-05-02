#include <app/s3App.h>
#include <app/s3Window.h>
#include <core/s3Settings.h>

// testing
#include <core/log/s3Log.h>

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
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

    case WM_SIZE:
    {
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

s3App::s3App()
{
    window = nullptr;
}

s3App::~s3App()
{
    S3_SAFE_DELETE(window);
}

bool s3App::init()
{
    window = new s3Window("Sophia", WindowProc, t3Vector2f(1280, 720), t3Vector2f(100, 100));
    if(!window)	
        return false;

    return true;
}

void s3App::shutdown()
{
    S3_SAFE_DELETE(window);
}

void s3App::render()
{
    s3Log::success("Hi Sophia\n");
}

void s3App::run()
{
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