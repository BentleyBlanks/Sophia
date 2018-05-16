#pragma once
#include <t3Vector2.h>
#include <core/s3Settings.h>

class s3Window
{
public:
    s3Window(const char* caption, WNDPROC proc, const t3Vector2f& size, const t3Vector2f& pos);
    ~s3Window();

    t3Vector2f getCursorPosition();
    t3Vector2f getWindowSize();
    void resize(int32 width, int32 height);

    t3Vector2f getWindowPosition();
    void setWindowPosition(int32 x, int32 y);

    HWND getHandle();

private:
    void adjustWindow();

    HWND  hWnd;
    DWORD style;
    t3Vector2f size;
    t3Vector2f position;
};