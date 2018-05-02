#include <Sophia.h>

int main()
{
    t3Timer timer;
    timer.start();
    s3Log::info("Whos' Your Daddy?\n");
    t3Vector3f a(10, 10, 10), b(20, 20, 20);
    a += b;

    timer.end();

    s3Log::success("Cost Time: %fs\n", timer.difference());

    a.print();

    s3App app;
    if(!app.init())
        return 0;

    app.run();

    //getchar();
    return 0;
}