#pragma once

#include <t3Vector2.h>

class s3Window;

class s3App
{
public:
    s3App();
    ~s3App();

    bool init(const t3Vector2f& size, const t3Vector2f& pos);
    void run();

    s3Window* getWindow();

private:
    void shutdown();
    void render();

    s3Window* window;
};