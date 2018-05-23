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

    s3Callback onMousePressed;
    s3Callback onMouseMoved;
    s3Callback onMouseReleased;
    s3Callback onMouseScrolled;
    s3Callback onKeyPressed;
    s3Callback onKeyReleased;
};

class s3CallbackManager
{
public:
    static s3Callbacks callBack;
};

// add / remove the system callbacks
void s3CallbackInit();
void s3CallbackDeinit();
