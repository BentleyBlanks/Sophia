#include <core/s3Random.h>

// --!See https://stackoverflow.com/questions/21237905/how-do-i-generate-thread-safe-uniform-random-numbers for more detail

#if (_MSC_VER <= 1800)
#define thread_local __declspec( thread )
#elif defined (__GCC__)
#define thread_local __thread
#endif

#include <random>
#include <time.h>
#include <thread>

/* Thread-safe function that returns a random number between min and max (inclusive).
This function takes ~142% the time that calling rand() would take. For this extra
cost you get a better uniform distribution and thread-safety. */

s3Random::s3Random()
{

}

double s3Random::randomDouble(const double min, const double max)
{
    static thread_local std::mt19937* generator = nullptr;

    if(!generator)
    {
        std::hash<std::thread::id> hasher;
        generator = new std::mt19937((unsigned int)(clock() + hasher(std::this_thread::get_id())));
    }
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(*generator);
}

float s3Random::randomFloat(const float min, const float max)
{
    static thread_local std::mt19937* generator = nullptr;

    if(!generator)
    {
        std::hash<std::thread::id> hasher;
        generator = new std::mt19937((unsigned int)(clock() + hasher(std::this_thread::get_id())));
    }
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(*generator);
}

int s3Random::randomInt(const int min, const int max)
{
    static thread_local std::mt19937* generator = nullptr;

    if(!generator)
    {
        std::hash<std::thread::id> hasher;
        generator = new std::mt19937((unsigned int)(clock() + hasher(std::this_thread::get_id())));
    }
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(*generator);
}
