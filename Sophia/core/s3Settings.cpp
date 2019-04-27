#include <core/s3Settings.h>
#include <stddef.h>
#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strstream>
#include <sstream>

s3Version atmosVersion;

std::string s3Version::get()
{
    std::ostringstream out;
    out << type << major << minor << revision;
    return out.str();
}

void s3Version::print()
{
    std::cout << "Atmos版本:" << type << " " << major << "." << minor << "." << revision << std::endl;
}

// Version Changes Level
s3Version::s3Version() : major(0), minor(0), revision(2), type("Alpha")
{

}
