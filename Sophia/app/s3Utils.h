#pragma once

#include <core/s3Settings.h>
#include <core/s3Event.h>
#include <t3Vector3.h>

enum s3ShaderType
{
    S3_VERTEX_SHADER,
    S3_GEOMETRY_SHADER,
    S3_HULL_SHADER,
    S3_PIXEL_SHADER
};

enum s3MouseState
{
    S3_MOUSE_PRESSED = 0,
    S3_MOUSE_RELEASED,
    S3_MOUSE_MOVED,
    S3_MOUSE_SCROLLED
};

enum s3KeyState
{
    S3_KEY_PRESSED = 0,
    S3_KEY_RELEASED
};

std::string s3MouseStateToString(s3MouseState state);

std::string s3KeyStateToString(s3KeyState state);

std::string s3MouseTypeToString(s3MouseEvent::s3ButtonType type);

std::string s3KeyTypeToString(s3KeyCode type);

std::string s3GetLatestProfile(s3ShaderType type);

s3ImageType s3GetImageType(const std::string & filePath);

float s3SphericalTheta(const t3Vector3f &w);

float s3SphericalPhi(const t3Vector3f &w);

// wstring string convert
std::wstring s3S2WS(const std::string& str);

std::string s3WS2S(const std::wstring& wstr);

// Copied from [OpenFrameworks](http://openframeworks.cc/)
// Converts all characters in a string to lowercase.
std::string s3ToLower(const std::string& src);

// Converts all characters in the string to uppercase.
std::string s3ToUpper(const std::string& src);

// Convert a value to a string.
// s3ToString does its best to convert any value to a string. If the data type
// implements a stream << operator, then it will be converted.
template <class T>
std::string s3ToString(const T& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}

//// bool need to return "true/false" not "1/0"
//template <>
//std::string s3ToString<bool>(const bool& b)
//{
//    if (b)
//        return "true";
//    else
//        return "false";
//}

// Convert a value to a string with a specific precision.
// Like sprintf "%4f" format, in this example precision=4
template <class T>
std::string s3ToString(const T& value, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

// Convert a value to a string with a specific width and fill
// Like sprintf "% 4d" or "% 4f" format, in this example width=4, fill=' '
template <class T>
std::string s3ToString(const T& value, int width, char fill)
{
    std::ostringstream out;
    out << std::fixed << std::setfill(fill) << std::setw(width) << value;
    return out.str();
}

// Convert a value to a string with a specific precision, width and filll
// Like sprintf "%04.2d" or "%04.2f" format, in this example precision=2, width=4, fill='0'
template <class T>
std::string s3ToString(const T& value, int precision, int width, char fill)
{
    std::ostringstream out;
    out << std::fixed << std::setfill(fill) << std::setw(width) << std::setprecision(precision) << value;
    return out.str();
}

// Convert a vector of values to a comma-delimited string.
// This method will take any vector of values and output a list of the values
// as a comma-delimited string.
template<class T>
std::string s3ToString(const std::vector<T>& values)
{
    std::stringstream out;
    int n = values.size();
    out << "{";
    if (n > 0)
    {
        for (int i = 0; i < n - 1; i++)
        {
            out << values[i] << ", ";
        }
        out << values[n - 1];
    }
    out << "}";
    return out.str();
}

// Convert a string to an integer.
int s3ToInt(const std::string& intString);

// Convert a string to a int64_t.
int64_t s3ToInt64(const std::string& intString);

// Convert a string to a float.
float s3ToFloat(const std::string& floatString);

// Convert a string to a double.
double s3ToDouble(const std::string& doubleString);

// Convert a string to a boolean.
bool s3ToBool(const std::string& boolString);

// Convert a string representation of a char to a actual char.
char s3ToChar(const std::string& charString);

// Converts any value to its equivalent hexadecimal representation.
// The hexadecimal representation corresponds to the way a number is stored in
// memory.
template <class T>
std::string s3ToHex(const T& value)
{
    std::ostringstream out;
    // pretend that the value is a bunch of bytes
    unsigned char* valuePtr = (unsigned char*)&value;
    // the number of bytes is determined by the datatype
    int numBytes = sizeof(T);
    // the bytes are stored backwards (least significant first)
    for (int i = numBytes - 1; i >= 0; i--)
    {
        // print each byte out as a 2-character wide hex value
        out << std::setfill('0') << std::setw(2) << std::hex << (int)valuePtr[i];
    }
    return out.str();
}

// Convert a string to a hexadecimal string.
template <class T>
std::string s3ToHex(const std::string& value)
{
    std::ostringstream out;
    // pretend that the value is a bunch of bytes
    unsigned char* valuePtr = (unsigned char*)&value;
    // the number of bytes is determined by the datatype
    int numBytes = sizeof(T);
    // the bytes are stored backwards (least significant first)
    for (int i = numBytes - 1; i >= 0; i--)
    {
        // print each byte out as a 2-character wide hex value
        out << std::setfill('0') << std::setw(2) << std::hex << (int)valuePtr[i];
    }
    return out.str();
}

// Convert a c-style string to a hexadecimal string.
std::string s3ToHex(const char* value);

// Convert a string representing an integer in hexadecimal to a string.
int s3HexToInt(const std::string& intHexString);

// Convert a string representing an char in hexadecimal to a char.
char s3HexToChar(const std::string& charHexString);

// Convert a string representing an float in hexadecimal to a float.
float s3HexToFloat(const std::string& floatHexString);

// Convert a string representing an string in hexadecimal to a string.
std::string s3HexToString(const std::string& stringHexString);

// Log
// reporting that the given function is not implemented
#define s3FuncNotImplementedError()\
{\
    std::string error = "Unimplemented ";\
    error += __FUNCTION__;\
    error += "() method called";\
    s3Log::error(error.c_str());\
}

// reporting a warning that ptr is nullptr
#define s3NullPtrWarning(ptrName)\
{\
    std::string error;\
    error += __FUNCTION__;\
    error += "() given ";\
    error += ptrName;\
    error += "is null";\
    a3Log::warning(error.c_str());\
}