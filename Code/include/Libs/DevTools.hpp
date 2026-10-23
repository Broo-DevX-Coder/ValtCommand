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
    
    // All devtools embeded functions
    namespace DevFunctions {
        
        void __init__(Runtime::RunTime& runtime); // Add all functions in runtime

        // All Inspectors ========
        ReturnResult<Value> scope_inspect(ExternalFunInType input); // Inspect scope
        ReturnResult<Value> vars_inspect(ExternalFunInType input); // Inspect vars
        ReturnResult<Value> funcs_inspect(ExternalFunInType input); // Inspect functions
 
    }
}