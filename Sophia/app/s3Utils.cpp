#include <app/s3Utils.h>
#include <app/s3Renderer.h>
#include <core/log/s3Log.h>

std::string s3MouseStateToString(s3MouseState state)
{
    switch (state)
    {
    case S3_MOUSE_PRESSED:
        return "s3MousePressed";
    case S3_MOUSE_RELEASED:
        return "s3MouseReleased";
    case S3_MOUSE_MOVED:
        return "s3MouseMoved";
    case S3_MOUSE_SCROLLED:
        return "s3MouseScrolled";
    default:
        return "Unknown";
    }
}

std::string s3KeyStateToString(s3KeyState state)
{
    switch (state)
    {
    case S3_KEY_PRESSED:
        return "s3KeyPressed";
    case S3_KEY_RELEASED:
        return "s3KeyReleased";
    default:
        return "Unknown";
    }
}

std::string s3MouseTypeToString(s3MouseEvent::s3ButtonType type)
{
    switch (type)
    {
    case s3MouseEvent::NONE:
        return "None";
    case s3MouseEvent::LEFT:
        return "Left";
    case s3MouseEvent::MIDDLE:
        return "Middle";
    case s3MouseEvent::RIGHT:
        return "Right";
    default:
        return "Unknown";
    }
}

std::string s3KeyTypeToString(s3KeyCode type)
{
    switch (type)
    {
    case None:
        return "None";
    case LButton:
        return "LButton";
    case RButton:
        return "RButton";
    case Cancel:
        return "Cancel";
    case MButton:
        return "MButton";
    case XButton1:
        return "XButton1";
    case XButton2:
        return "XButton2";
    case Back:
        return "Back";
    case Tab:
        return "Tab";
    case Clear:
        return "Clear";
    case Enter:
        return "Enter";
    case ShiftKey:
        return "ShiftKey";
    case ControlKey:
        return "ControlKey";
    case AltKey:
        return "AltKey";
    case Pause:
        return "Pause";
    //case Capital:
    //    return "Capital";
    case CapsLock:
        return "CapsLock";
    case KanaMode:
        return "KanaMode";
    //case HanguelMode:
    //    return "HanguelMode";
    //case HangulMode:
    //    return "HangulMode";
    case JunjaMode:
        return "JunjaMode";
    case FinalMode:
        return "FinalMode";
    case HanjaMode:
        return "HanjaMode";
    //case KanjiMode:
    //    return "KanjiMode";
    case EscapeKey:
        return "EscapeKey";
    case IMEConvert:
        return "IMEConvert";
    case IMINoConvert:
        return "IMINoConvert";
    case IMEAccept:
        return "IMEAccept";
    case IMIModeChange:
        return "IMIModeChange";
    case Space:
        return "Space";
    //case Prior:
    //    return "Prior";
    case PageUp:
        return "PageUp";
    //case Next:
    //    return "Next";
    case PageDown:
        return "PageDown";
    case End:
        return "End";
    case Home:
        return "Home";
    case Left:
        return "Left";
    case Up:
        return "Up";
    case Right:
        return "Right";
    case Down:
        return "Down";
    case Select:
        return "Select";
    case Print:
        return "Print";
    case Execute:
        return "Execute";
    case PrintScreen:
        return "PrintScreen";
    //case Snapshot:
    //    return "Snapshot";
    case Insert:
        return "Insert";
    case Delete:
        return "Delete";
    case Help:
        return "Help";
    case D0:
        return "D0";
    case D1:
        return "D1";
    case D2:
        return "D2";
    case D3:
        return "D3";
    case D4:
        return "D4";
    case D5:
        return "D5";
    case D6:
        return "D6";
    case D7:
        return "D7";
    case D8:
        return "D8";
    case D9:
        return "D9";
    case A:
        return "A";
    case B:
        return "B";
    case C:
        return "C";
    case D:
        return "D";
    case E:
        return "E";
    case F:
        return "F";
    case G:
        return "G";
    case H:
        return "H";
    case I:
        return "I";
    case J:
        return "J";
    case K:
        return "K";
    case L:
        return "L";
    case M:
        return "M";
    case N:
        return "N";
    case O:
        return "O";
    case P:
        return "P";
    case Q:
        return "Q";
    case R:
        return "R";
    case S:
        return "S";
    case T:
        return "T";
    case U:
        return "U";
    case V:
        return "V";
    case W:
        return "W";
    case X:
        return "X";
    case Y:
        return "Y";
    case Z:
        return "Z";
    case LWin:
        return "LWin";
    case RWin:
        return "RWin";
    case Apps:
        return "Apps";
    case SleepKey:
        return "SleepKey";
    case NumPad0:
        return "NumPad0";
    case NumPad1:
        return "NumPad1";
    case NumPad2:
        return "NumPad2";
    case NumPad3:
        return "NumPad3";
    case NumPad4:
        return "NumPad4";
    case NumPad5:
        return "NumPad5";
    case NumPad6:
        return "NumPad6";
    case NumPad7:
        return "NumPad7";
    case NumPad8:
        return "NumPad8";
    case NumPad9:
        return "NumPad9";
    case Multiply:
        return "Multiply";
    case Add:
        return "Add";
    case Separator:
        return "Separator";
    case Subtract:
        return "Subtract";
    case Decimal:
        return "Decimal";
    case Divide:
        return "Divide";
    case F1:
        return "F1";
    case F2:
        return "F2";
    case F3:
        return "F3";
    case F4:
        return "F4";
    case F5:
        return "F5";
    case F6:
        return "F6";
    case F7:
        return "F7";
    case F8:
        return "F8";
    case F9:
        return "F9";
    case F10:
        return "F10";
    case F11:
        return "F11";
    case F12:
        return "F12";
    case F13:
        return "F13";
    case F14:
        return "F14";
    case F15:
        return "F15";
    case F16:
        return "F16";
    case F17:
        return "F17";
    case F18:
        return "F18";
    case F19:
        return "F19";
    case F20:
        return "F20";
    case F21:
        return "F21";
    case F22:
        return "F22";
    case F23:
        return "F23";
    case F24:
        return "F24";
    case NumLock:
        return "NumLock";
    case Scroll:
        return "Scroll";
    case LShiftKey:
        return "LShiftKey";
    case RShiftKey:
        return "RShiftKey";
    case LControlKey:
        return "LControlKey";
    case RControlKey:
        return "RControlKey";
    case LMenu:
        return "LMenu";
    case RMenu:
        return "RMenu";
    case BrowserBack:
        return "BrowserBack";
    case BrowserForward:
        return "BrowserForward";
    case BrowserRefresh:
        return "BrowserRefresh";
    case BrowserStop:
        return "BrowserStop";
    case BrowserSearch:
        return "BrowserSearch";
    case BrowserFavorites:
        return "BrowserFavorites";
    case BrowserHome:
        return "BrowserHome";
    case VolumeMute:
        return "VolumeMute";
    case VolumeDown:
        return "VolumeDown";
    case VolumeUp:
        return "VolumeUp";
    case MediaNextTrack:
        return "MediaNextTrack";
    case MediaPreviousTrack:
        return "MediaPreviousTrack";
    case MediaStop:
        return "MediaStop";
    case MediaPlayPause:
        return "MediaPlayPause";
    case LaunchMail:
        return "LaunchMail";
    case SelectMedia:
        return "SelectMedia";
    case LaunchApplication1:
        return "LaunchApplication1";
    case LaunchApplication2:
        return "LaunchApplication2";
    case OemSemicolon:
        return "OemSemicolon";
    //case Oem1:
    //    return "Oem1";
    case OemPlus:
        return "OemPlus";
    case OemComma:
        return "OemComma";
    case OemMinus:
        return "OemMinus";
    case OemPeriod:
        return "OemPeriod";
    case OemQuestion:
        return "OemQuestion";
    //case Oem2:
    //    return "Oem2";
    case OemTilde:
        return "OemTilde";
    //case Oem3:
    //    return "Oem3";
    case OemOpenBrackets:
        return "OemOpenBrackets";
    //case Oem4:
    //    return "Oem4";
    case OemPipe:
        return "OemPipe";
    //case Oem5:
    //    return "Oem5";
    case OemCloseBrackets:
        return "OemCloseBrackets";
    //case Oem6:
    //    return "Oem6";
    case OemQuotes:
        return "OemQuotes";
    //case Oem7:
    //    return "Oem7";
    case Oem8:
        return "Oem8";
    case OemBackslash:
        return "OemBackslash";
    //case Oem102:
    //    return "Oem102";
    case ProcessKey:
        return "ProcessKey";
    case Packet:
        return "Packet";
    case Attn:
        return "Attn";
    case CrSel:
        return "CrSel";
    case ExSel:
        return "ExSel";
    case EraseEof:
        return "EraseEof";
    case Play:
        return "Play";
    case Zoom:
        return "Zoom";
    case NoName:
        return "NoName";
    case Pa1:
        return "Pa1";
    case OemClear:
        return "OemClear";
    default:
        return "Unknow";
    }
}

std::string s3BoolToString(bool b)
{
    if (b)
        return "true";
    else
        return "false";
}

std::string s3GetLatestProfileVertexShader(D3D_FEATURE_LEVEL featureLevel)
{
    switch(featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "vs_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "vs_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "vs_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "vs_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "vs_4_0_level_9_1";
    }
    break;
    } // switch( featureLevel )

    return "";
}

std::string s3GetLatestProfilePixelShader(D3D_FEATURE_LEVEL featureLevel)
{
    switch(featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "ps_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "ps_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "ps_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "ps_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "ps_4_0_level_9_1";
    }
    break;
    }
    return "";
}

std::string s3GetLatestProfile(s3ShaderType type)
{
    ID3D11Device* device = s3Renderer::get().getDevice();
    if(!device)
    {
        s3Log::warning("s3GetLatestProfile():Need to initialze the renderer first\n");
        return "";
    }

    D3D_FEATURE_LEVEL featureLevel = device->GetFeatureLevel();

    switch(type)
    {
    case S3_VERTEX_SHADER:
        return s3GetLatestProfileVertexShader(featureLevel);
    case S3_PIXEL_SHADER:
        return s3GetLatestProfilePixelShader(featureLevel);
    case S3_GEOMETRY_SHADER:
    case S3_HULL_SHADER:
    default:
        return "";
    }
}

s3ImageType s3GetImageType(const std::string & filePath)
{
    s3ImageType type;
    char extension[32];
    _splitpath(filePath.c_str(), NULL, NULL, NULL, extension);

    // parsing image file's postfix
    if (!_stricmp(extension, ".png"))
        type = S3_IMAGE_PNG;
    else if (!_stricmp(extension, ".exr"))
        type = S3_IMAGE_EXR;
    else if (!_stricmp(extension, ".hdr"))
        type = S3_IMAGE_HDR;
    else
        type = S3_IMAGE_ERROR;

    return type;
}

float s3SphericalTheta(const t3Vector3f &v)
{
    return t3Math::acosRad(t3Math::clamp(v.y, -1.f, 1.f));
}

float s3SphericalPhi(const t3Vector3f &v)
{
    float p = t3Math::atan2Rad(v.z, v.x);
    return (p < 0.f) ? p + 2.f * T3MATH_PI : p;
}