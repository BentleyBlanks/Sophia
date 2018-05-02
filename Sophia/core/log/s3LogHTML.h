#pragma once

#include <core/log/s3Log.h>

// not support UTF8 log to file
// default file name format [year]-[month]-[day]-Atmos-VertionType.html
// would cover the same name file, be sure to end() when everything is finished
class s3LogHTML
{
public:
    static void log(s3LogLevel logLevel, const char* message, ...);

    static void begin();

    static void end();

    // log to default name file: [prefix]:Atmos-VerisionType.html
    static void fatalError(const char* message, ...);

    static void error(const char* message, ...);

    static void seriousWarning(const char* message, ...);

    static void warning(const char* message, ...);

    static void success(const char* message, ...);

    static void info(const char* message, ...);

    static void dev(const char* message, ...);

    static void debug(const char* message, ...);

private:
    s3LogHTML() {}
    s3LogHTML(const s3LogHTML&) {}
    ~s3LogHTML() {}
    s3LogHTML& operator=(const s3LogHTML& event) const {}

    static void log(s3LogLevel logLevel, const char* message, va_list args);

    static std::ofstream* ofile;

    static bool isBegin;
};