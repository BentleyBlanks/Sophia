#include <core/s3Callback.h>

void s3Callback::operator+=(s3CallbackHandle& handle)
{
    handles.push_back(&handle);
}

void s3Callback::operator-=(s3CallbackHandle& handle)
{
    // remove specific handle in array
    for(std::vector<s3CallbackHandle*>::iterator it = handles.begin(); it != handles.end(); )
    {
        if(*it == &handle)
            it = handles.erase(it);
        else
            ++it;
    }
}

void s3Callback::trigger(s3CallbackUserData* data)
{
    s3CallbackUserData defaultUserData;
    if(!data)
        data = &defaultUserData;
    data->sender = this;

    for(int i = 0; i < handles.size(); i++)
    {
        handles[i]->onHandle(data);
    }
}
