#pragma once

class s3Window;

class s3App
{
public:
    s3App();
    ~s3App();

    bool init();
    void run();

private:
    void shutdown();
    void render();

    s3Window* window;
};