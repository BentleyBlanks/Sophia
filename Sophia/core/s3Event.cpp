#include <core/s3Event.h>
#include <core/log/s3Log.h>
#include <app/s3Utils.h>

s3MouseEvent::s3MouseEvent(s3ButtonType type, int32 x, int32 y, int32 offsetX, int32 offsetY, float32 scroll, bool control, bool shift)
    : 
    type(type),
    x(x), y(y), 
    offsetX(offsetX), offsetY(offsetY),
    scroll(scroll),
    control(control), shift(shift)
{
}

void s3MouseEvent::print(int state) const
{
    s3Log::debug("%s type: %s, pos: [%d, %d], offset: [%d, %d], scroll: %.1f, control: %s, shift: %s\n",
        s3MouseStateToString((s3MouseState) state).c_str(), 
        s3MouseTypeToString(type).c_str(), x, y, offsetX, offsetY, scroll, s3ToString(control).c_str(), s3ToString(shift).c_str());
}

s3KeyEvent::s3KeyEvent(s3KeyCode keyCode, uint32 key, bool control, bool shift, bool alt)
    :
    keyCode(keyCode), key(key),
    control(control), shift(shift), alt(alt)
{
}

void s3KeyEvent::print(int state) const
{
    if(key == 0)
        s3Log::debug("%s code: %s, key: , control: %s, shift: %s, alt: %s\n",
        s3KeyStateToString((s3KeyState) state).c_str(), 
        s3KeyTypeToString(keyCode).c_str(), s3ToString(control).c_str(), s3ToString(shift).c_str(), s3ToString(alt).c_str());
    else
        s3Log::debug("%s code: %s, key: %c, control: %s, shift: %s, alt: %s\n", 
        s3KeyStateToString((s3KeyState)state).c_str(), 
        s3KeyTypeToString(keyCode).c_str(), (uint8) key, s3ToString(control).c_str(), s3ToString(shift).c_str(), s3ToString(alt).c_str());
}
