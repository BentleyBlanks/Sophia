#pragma once

class s3Window;

class s3App
{
public:
    s3App();
    ~s3App();

    bool init();
    void run();

    s3Window* getWindow();

private:
    void shutdown();
    void render();

    s3Window* window;
};