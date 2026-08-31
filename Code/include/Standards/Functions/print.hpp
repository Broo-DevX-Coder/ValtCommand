#pragma once

// ==================================================================
// Marcos
// ==================================================================
#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define API __declspec(dllimport)
    #else
        #define API 
    #endif
#endif

// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <iostream>

// == Locals ==
#include "Runtime.hpp"

// ==================================================================
// Namespace
// ==================================================================

namespace Standardes {
    namespace Functions {
        API void print(Runtime::FunInType input); // Print data in console
    }
}