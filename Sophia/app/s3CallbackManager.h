#pragma once

#include <core/s3Callback.h>

class s3Callbacks
{
public:
    s3Callback onEngineInit;
    s3Callback onEngineDeinit;
    s3Callback onBeginRender;
    s3Callback onEndRender;
    s3Callback onUpdate;
};

class s3CallbackManager
{
public:
    static s3Callbacks callBack;
};

// add / remove the system callbacks
void s3CallbackInit();
void s3CallbackDeinit();
