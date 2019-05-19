#pragma once

// ------------------------Header------------------------
#include <iostream>
#include <sstream>
// std's lib
#include <string>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <map>
// min&max function
#include <algorithm>
#include <assert.h>
#include <math.h>
// DirectX 
#include <d3d11.h>
#include <d3dCompiler.h>

//--! https://stackoverflow.com/questions/20446373/cin-ignorenumeric-limitsstreamsizemax-n-max-not-recognize-it?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
// After all the includes, before any code
#undef max

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


// ------------------------Type Renaming------------------------
#ifndef int64
#define int64	signed long long
#endif

#ifndef uint64
#define uint64	unsigned long long
#endif

#ifndef int32
#define int32	int
#endif

#ifndef uint32
#define uint32	unsigned int
#endif

#ifndef int16
#define int16	signed short
#endif

#ifndef uint16
#define uint16	unsigned short
#endif

#ifndef int8
#define int8	signed char
#endif

#ifndef uint8
#define uint8	unsigned char
#endif

#ifndef float32
#define float32	float
#endif

#ifndef float64
#define float64	double
#endif

#ifndef ushort
#define ushort	unsigned short int
#endif

enum s3ImageType
{
    S3_IMAGE_ERROR = -1,
    S3_IMAGE_PNG = 0,
    S3_IMAGE_EXR = 1,
    S3_IMAGE_HDR = 2
};

// ------------------------Macro Function------------------------
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

template<typename T>
void S3_SAFE_RELEASE(T*& x)
{
    if(x)
    {
        x->Release(); 
        x = nullptr;
    }
}

// ------------------------Assert------------------------
#ifndef S3ASSERT
#define S3ASSERT(exp, errorMessage) assert(exp && errorMessage)
#endif

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

#ifndef S3_DEGREE_TO_RADIAN
#define S3_DEGREE_TO_RADIAN (PI/180.0f)
#endif

#ifndef S3_RADIAN_TO_DEGREE
#define S3_RADIAN_TO_DEGREE (180.0f/PI)
#endif

// Correct the computation of float error 
#define S3_EPSILON 1e-4f
#define S3_EPSILON_FLOAT 1e-6f
#define S3_EPSILON_DOUBLE 1e-8

// Maximim Constants
#ifndef S3_MAX_INT8
#define S3_MAX_INT8	((int8)		0x7f)
#endif

#ifndef S3_MAX_INT16
#define S3_MAX_INT16	((int16)	0x7fff)
#endif

#ifndef S3_MAX_INT32
#define S3_MAX_INT32	((int32)	0x7fffffff)
#endif

#ifndef S3_MAX_INT64
#define S3_MAX_INT64	((int64)	0x7fffffffffffffff)
#endif

#ifndef S3_MAX_UINT8
#define S3_MAX_UINT8	((uint8)	0xff)
#endif

#ifndef S3_MAX_UINT16
#define S3_MAX_UINT16	((uint16)	0xffff)
#endif

#ifndef S3_MAX_UINT32
#define S3_MAX_UINT32	((uint32)	0xffffffff)
#endif

#ifndef S3_MAX_UINT64
#define S3_MAX_UINT64	((uint64)	0xffffffffffffffff)
#endif

#ifndef S3_INFINITY
#define S3_INFINITY FLT_MAX
#endif

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

// 当前版本号
extern s3Version atmosVersion;
