#pragma once
#pragma once

// ------------------------Platforms------------------------
// refer to http://www.ogre3d.org/docs/api/html/OgrePlatform_8h-source.html
#if defined( __WIN32__ ) || defined( _WIN32 )
#define S3_PLATFORM_WINDOWS
#include <windows.h>

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define WIN32_LEAN_AND_MEAN
// --!max macro's error
// --!http://stackoverflow.com/questions/1904635/warning-c4003-and-errors-c2589-and-c2059-on-x-stdnumeric-limitsintmax

// Apple platform
#elif defined( __APPLE_CC__)
#include <TargetConditionals.h>

// iOS platform(without iPad)
#if (TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
#define S3_PLATFORM_IPHONE
#define S3_PLATFORM_IOS
#define S3_PLATFORM_OPENGLES

// Mac platform
#else
#define S3_PLATFORM_OSX
#endif

// Android platform
#elif defined (__ANDROID__)
#define S3_PLATFORM_ANDROID
#define S3_PLATFORM_OPENGLES

// Linux platform
#elif defined(__ARMEL__)
#define S3_PLATFORM_LINUX
#define S3_PLATFORM_OPENGLES
#define S3_PLATFORM_LINUX_ARM
#else
#define S3_PLATFORM_LINUX
#endif

#include <iostream>
#include <sstream>
// std's lib
#include <string>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
// min&max function
#include <algorithm>
#include <assert.h>
#include <math.h>

//--! https://stackoverflow.com/questions/20446373/cin-ignorenumeric-limitsstreamsizemax-n-max-not-recognize-it?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
// After all the includes, before any code
#undef max

template<typename T>
void S3_SAFE_DELETE(T*& a)
{
    if(a)
    {
        delete a;
        a = NULL;
    }
}

template<typename T>
void S3_SAFE_DELETE_1DARRAY(T*& pointer)
{
    if(pointer)
    {
        delete[] pointer;
        pointer = NULL;
    }
}

// ------------------------Assert------------------------
#ifndef S3ASSERT
#define S3ASSERT(exp, errorMessage) assert(exp && errorMessage)
#endif

// ------------------------Deprecated Macro------------------------
#ifdef S3_IGNORE_DEPRECATION_GLOBALLY

#define S3_DEPRECATED
#define S3_DEPRECATED
#define S3_DISABLE_DEPRECATION
#define S3_ENABLE_DEPRECATION

#endif

// GCC
#if defined(__GCC__) || defined(__SNC__) || defined(__GNUC__)
#define S3_DEPRECATED __attribute__ ((deprecated))

///<MSVC 7.0
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define  S3_DEPRECATED __declspec(deprecated)

#else
///< CWCC doesn't seem to have such a 'marker' facility.
#define S3_DEPRECATED 
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1300) ///<MSVC 7.0
// On Windows this macro will mark code as deprecated.
#define S3_DEPRECATED __declspec(deprecated)

// This macro will temporarily disable the deprecated warning, allowing you to use deprecated code without getting a warning.
#define S3_DISABLE_DEPRECATION \
    __pragma (warning(push)) \
    __pragma (warning(disable : 4996))

// This macro will enable the deprecated warning again, this should be used at the end of a code block which had this warning disabled.
#define S3_ENABLE_DEPRECATION \
    __pragma (warning(pop))
#else
// On Non-Windows platforms this macro is ignored.
#define S3_DEPRECATED
// On Non-Windows platforms this macro is ignored.
#define S3_DISABLE_DEPRECATION
// On Non-Windows platforms this macro is ignored.
#define S3_ENABLE_DEPRECATION
#endif

// ¶ÏÑÔ
#ifdef NDEBUG
#define s3Assert(expr) ((void)0)
#else
#define s3Assert(expr) \
    ((expr) ? (void)0 : \
        printf("Assertion \"%s\" failed in %s, line %d", \
               #expr, __FILE__, __LINE__))
#endif // NDEBUG

// ------------------------Math Constants------------------------
#ifndef PI
#define PI 3.1415926535897
#endif

#define S3_SQRT2 1.4142135623730950488016887242097
#define S3_SQRT3 1.7320508075688772935274463415059

#define S3_EXIT(value) std::exit(value);

#ifndef S3_INFINITY
#define S3_INFINITY FLT_MAX
#endif

// math
#ifndef S3_DEGREE_TO_RADIAN
#define S3_DEGREE_TO_RADIAN (PI/180.0f)
#endif

#ifndef S3_RADIAN_TO_DEGREE
#define S3_RADIAN_TO_DEGREE (180.0f/PI)
#endif

// Correct the computation of float error 
#define S3_EPSILON 1e-4f
#define S3_EPSILON_INT 0
#define S3_EPSILON_FLOAT 1e-6f
#define S3_EPSILON_DOUBLE 1e-8
// for shadow ray testing
#define S3_EPSILON_SHADOW 1e-3f
//for roundoff errors
#define S3_EPSILON_DELTA 1e-3f

// ------------------------Version------------------------
// Sophia's Verision info
struct s3Version
{
    s3Version();

    std::string get();

    void print();

    int major;		// significant changes

    int minor;		// incremental changes

    int revision;		// bug fixes

    std::string type;   // build type
};

// µ±Ç°°æ±¾ºÅ
extern s3Version atmosVersion;
