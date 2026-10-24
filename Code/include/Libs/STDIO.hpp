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

// == Locals ==
#include "API/Runtime.hpp"

// == Libs ==
#include <functional>

// ==================================================================
// Namespace
// ==================================================================
namespace Lib{
    
    // All standard input output functions
    namespace STDIO {
        
        void __init__(Runtime::RunTime& runtime); // Add all functions in runtime

        // Output ========
        ReturnResult<Value> print_fn(ExternalFunInType input);   // print(a, b, ...) without newline
        ReturnResult<Value> println_fn(ExternalFunInType input); // println(a, b, ...) with newline

        // Input ========
        ReturnResult<Value> input_fn(ExternalFunInType input);   // input(prompt?) -> string
 
    }
}