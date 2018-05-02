#include <core/log/s3LogHTML.h>
#include <stddef.h>
#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strstream>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream> 
#include <fstream>
#include <iomanip>
#include <core/s3Settings.h>

#ifdef S3_PLATFORM_WINDOWS
#define NOMINMAX 
#include <Windows.h>
#endif

#define CHECK_IFSTARTED \
    if(!s3LogHTML::isBegin) { \
        printf("System Error: Call a3LogHTML::log() without begin()\n"); \
        return; } 

bool s3LogHTML::isBegin = false;

std::ofstream* s3LogHTML::ofile = new std::ofstream();

//std::ifstream* a3LogHTML::ifile = new std::ifstream();

std::vector<std::string> logList;

void s3LogHTML::begin()
{
    isBegin = true;

    static char name[128];
    time_t cur;
    const struct tm* tm;

    cur = time(NULL);
    tm = localtime(&cur);

    sprintf(name, "%02d-%02d-%02d-Atmos-%s.html", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, atmosVersion.get().c_str());

    ofile->open(name);

    logList.push_back("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>Atmos Log</title></head><body>");

    logList.push_back("<font color=\"#000000\"> <br> === Atmos Log Start === <br><br><br><br> </font>");
}

void s3LogHTML::end()
{
    CHECK_IFSTARTED

    logList.push_back("<font color=\"#000000\"><br><br><br> === Atmos Log End === <br> </font></body></html>");
    
    for(auto l : logList)
        *ofile << l;

    ofile->close();
}

void s3LogHTML::log(s3LogLevel logLevel, const char* message, va_list args)
{
    const char* lv;
    time_t cur = time(NULL);
    const struct tm* tm = localtime(&cur);
    static char content[1024], timeBuffer[64], messageBuffer[1024], contentBuffer[1024];

    switch(logLevel)
    {
    case S3_LOG_LEVEL_FATALERROR: lv = "Fatal Error"; break;
    case S3_LOG_LEVEL_ERROR: lv = "Error"; break;
    case S3_LOG_LEVEL_SERIOUSWARNING: lv = "Serious Warning"; break;
    case S3_LOG_LEVEL_WARNING: lv = "Warning"; break;
    case S3_LOG_LEVEL_SUCCESS: lv = "Success"; break;
    case S3_LOG_LEVEL_INFO: lv = "Info"; break;
    case S3_LOG_LEVEL_DEV: lv = "Dev"; break;
    case S3_LOG_LEVEL_DEBUG: lv = "Debug"; break;
    default: lv = "Other"; break;
    }

    sprintf(timeBuffer, "[%02d:%02d:%02d]        [%s]        ", tm->tm_hour, tm->tm_min, tm->tm_sec, lv);
    // messageBuffer:[hour]:[minute]:[second]
    strcpy(messageBuffer, timeBuffer);
    // messageBuffer:[hour]:[minute]:[second] message ...
    strcat(messageBuffer, message);
    // messageBuffer:[hour]:[minute]:[second] message'
    vsprintf(contentBuffer, messageBuffer, args);

    std::string msg(contentBuffer);

    std::size_t found = msg.find("&");
    if(found != std::string::npos)
        msg.replace(found, 1, "$amp");

    found = msg.find("<");
    if(found != std::string::npos)
        msg.replace(found, 1, "&lt");

    found = msg.find(">");
    if(found != std::string::npos)
        msg.replace(found, 1, "&gt");

    found = msg.find("\n");
    if(found != std::string::npos)
        msg.replace(found, 1, "<br>");

    switch(logLevel)
    {
    case s3LogLevel::S3_LOG_LEVEL_BEGINBLOCK:
        sprintf(content, "<br><font color=\"#8080FF\"><b> <u>%s</u> </b> (%s) </font><table width=100%% border=0><tr width=100%%><td width=10></td><td width=*>\n", msg.c_str(), "tag");
        break;
    case s3LogLevel::S3_LOG_LEVEL_ENDBLOCK:
        sprintf(content, "</td></tr></table><font color=\"#8080FF\"><b> %s </b></font>\n", "");
        break;
    case s3LogLevel::S3_LOG_LEVEL_FATALERROR:
        sprintf(content, "<font color=\"#FF0000\"><b>%s</b></font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_ERROR:
        sprintf(content, "<font color=\"#FF0000\"><b>%s</b></font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_SERIOUSWARNING:
        sprintf(content, "<font color=\"#FF4000\"><b>%s</b></font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_WARNING:
        sprintf(content, "<font color=\"#FF8000\">%s</font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_SUCCESS:
        sprintf(content, "<font color=\"#009000\">%s</font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_INFO:
        sprintf(content, "<font color=\"#000000\">%s</font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_DEV:
        sprintf(content, "<font color=\"#3030F0\">%s</font><br>\n", msg.c_str());
        break;
    case s3LogLevel::S3_LOG_LEVEL_DEBUG:
        sprintf(content, "<font color=\"#00FFFF\">%s</font><br>\n", msg.c_str());
        break;
    default:
        return;
    }

    logList.push_back(std::string(content));
}

void s3LogHTML::log(s3LogLevel logLevel, const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(logLevel, message, args);
    va_end(args);
}

void s3LogHTML::fatalError(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_FATALERROR, message, args);
    va_end(args);
}

void s3LogHTML::error(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_ERROR, message, args);
    va_end(args);
}

void s3LogHTML::seriousWarning(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_SERIOUSWARNING, message, args);
    va_end(args);
}

void s3LogHTML::warning(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_WARNING, message, args);
    va_end(args);
}

void s3LogHTML::success(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_SUCCESS, message, args);
    va_end(args);
}

void s3LogHTML::info(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_INFO, message, args);
    va_end(args);
}

void s3LogHTML::dev(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_DEV, message, args);
    va_end(args);
}

void s3LogHTML::debug(const char* message, ...)
{
    CHECK_IFSTARTED

    va_list args;
    va_start(args, message);
    log(S3_LOG_LEVEL_DEBUG, message, args);
    va_end(args);
}









