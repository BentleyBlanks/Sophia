#pragma once 

#include <core/s3Settings.h>

enum s3LogLevel
{
    S3_LOG_LEVEL_BEGINBLOCK = -2,
    S3_LOG_LEVEL_ENDBLOCK = -1,
    S3_LOG_LEVEL_NONE = 0, // not display to the console
    S3_LOG_LEVEL_FATALERROR = 1,
    S3_LOG_LEVEL_ERROR = 2,
    S3_LOG_LEVEL_SERIOUSWARNING = 3,
    S3_LOG_LEVEL_WARNING = 4,
    S3_LOG_LEVEL_SUCCESS = 5,
    S3_LOG_LEVEL_INFO = 6,
    S3_LOG_LEVEL_DEV = 7,
    S3_LOG_LEVEL_DEBUG = 8,
    S3_LOG_LEVEL_ALLMESSAGES = 9 // Placeholder
};

// not support UTF8 log to file
class s3Log
{
public:
    static void log(s3LogLevel logLevel, const char* message, ...);

    // log to console / file
    static void fatalError(const char* message, ...);

    static void error(const char* message, ...);

    static void seriousWarning(const char* message, ...);

    static void warning(const char* message, ...);

    static void success(const char* message, ...);

    static void info(const char* message, ...);

    static void dev(const char* message, ...);

    static void debug(const char* message, ...);

    // replacement for printf
    static void print(const char* message, ...);
private:
    s3Log() {}
    s3Log(const s3Log&) {}
    ~s3Log() {}
    s3Log& operator=(const s3Log& event) const {}

    static void log(s3LogLevel logLevel, const char* message, va_list args);
};


// 函数调用error
#define a3FunctionError(errorMessage) a3FunctionError_<int>(std::string(__FUNCTION__), errorMessage)

#define a3FunctionErrorArg(argName, arg, errorMessage) a3FunctionError_(std::string(__FUNCTION__), argName, arg, errorMessage)

template<class T>
void a3FunctionError_(std::string functionName, std::string argName, T arg, std::string errorMessage)
{
    std::cout << "Error: " << "happened when excute: " << functionName << "() with parameter：" << argName << " = " << arg << "and error message: " << errorMessage
        << std::endl;
}

template<class T>
void a3FunctionError_(std::string functionName, std::string errorMessage)
{
    std::cout << "Error: " << "happened when excute: " << functionName << "() with error message：" << errorMessage << std::endl;
}