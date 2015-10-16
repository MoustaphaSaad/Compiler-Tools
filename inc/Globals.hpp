#pragma once

#include <stdint.h>
#include <cstddef>

#if defined(WIN32)||defined(_WIN32)||defined(__WIN32__)||defined(_WIN64)||defined(WIN64)||defined(__MINGW32__)||defined(__MINGW64__)
    #define OS_WINDOWS
#elif defined(__linux__) || defined(__unix__)
    #define OS_LINUX
#endif


#ifdef OS_WINDOWS
#ifdef COMPILE_DLL
    #define API __declspec(dllexport)
#else
	#define API __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
    #define API
#endif

namespace CTools{
	typedef unsigned char byte;

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float real32;
    typedef double real64;
    
    static const u32 MEMORY_SIZE = 25 * 1024 * 1024;
}