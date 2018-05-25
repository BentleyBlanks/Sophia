#include <app/s3Window.h>

s3Window::s3Window(const char * caption, WNDPROC proc, const t3Vector2f& size, const t3Vector2f& pos)
{
    this->hWnd = 0;
    this->size = size;
    this->position = pos;
    this->style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE);

    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = 0;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = caption;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                            wc.lpszClassName,
                            caption,
                            style,
                            (int32) position.x,
                            (int32) position.y,
                            (int32) size.x,
                            (int32) size.y,
                            NULL,
                            NULL,
                            NULL,
                            NULL);

    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
}

s3Window::~s3Window()
{
    if(hWnd)	
        DestroyWindow(hWnd);
    hWnd = NULL;
}

t3Vector2f s3Window::getCursorPosition()
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);

    return t3Vector2f(static_cast< float32 >(p.x), static_cast< float32 >(p.y));
}

t3Vector2f s3Window::getWindowSize()
{
    return size;
}

void s3Window::resize(int32 width, int32 height)
{
    size.x = (float32) width;
    size.y = (float32) height;
}

t3Vector2f s3Window::getWindowPosition()
{
    return position;
}

void s3Window::setWindowPosition(int32 x, int32 y)
{
    position.x = (float32) x;
    position.y = (float32) y;

    adjustWindow();
}

HWND s3Window::getHandle()
{
    return hWnd;
}

void s3Window::adjustWindow()
{
    RECT rect;
    rect.left = (LONG) position.x;
    rect.top = (LONG) position.y;
    rect.right = (LONG) (position.x +size.x);
    rect.bottom = (LONG) (position.y + size.y);
    AdjustWindowRect(&rect, style, false);
}

