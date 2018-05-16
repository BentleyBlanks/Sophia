#pragma once

#include <vector>

struct s3CallbackUserData
{
    void* sender = nullptr;
    void* userData = nullptr;
};

class s3CallbackHandle
{
public:
    virtual void onHandle(const s3CallbackUserData* userData) = 0;
};

class s3Callback
{
public:
    void operator+=(s3CallbackHandle& handle);
    void operator-=(s3CallbackHandle& handle);

    void trigger(s3CallbackUserData* data = nullptr);

protected:
    std::vector<s3CallbackHandle*> handles;
};