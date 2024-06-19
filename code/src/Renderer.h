#pragma once

#include <GL/glew.h>

// Cross-platform __debugbreak definition
#ifdef _MSC_VER
    #define __debugbreak() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #include <signal.h>
    #define __debugbreak() raise(SIGTRAP)
#else
    #define __debugbreak() ((void)0)
#endif

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char * function, const char * file, int line);
