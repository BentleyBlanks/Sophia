#include <Sophia.h>
#include <core/s3Settings.h>
#include <core/log/s3Log.h>
#include <Common/t3Timer.h>
#include <t3Math/core/t3Vector3.h>

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
    getchar();
    return 0;
}