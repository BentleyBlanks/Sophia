#pragma once

#include <core/s3Settings.h>
#include <t3Vector2.h>
#include <t3Vector4.h>
#include <Common/t3Timer.h>

class s3Window;

class s3App
{
public:
    s3App();
    ~s3App();

    bool init(const t3Vector2f& size, const t3Vector2f& pos);
    void run();
    void setClearColor(t3Vector4f clearColor);

    s3Window* getWindow();

	// get elapsed time when app is running
	float32 getTimeElapsed();
private:
    void shutdown();
    void render();

    s3Window* window;
    t3Vector4f clearColor;

	t3Timer timer;
};