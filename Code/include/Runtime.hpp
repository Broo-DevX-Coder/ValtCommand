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
#include "globals.hpp"

// == Libs ==
#include <functional>

// ==================================================================
// Namespace
// ==================================================================
namespace Runtime {

    using FunInType = std::unordered_map<std::string, Value>;
    using FuncType = std::function<ReturnResult<Value>(FunInType)>;

    namespace registries {
        API extern std::unordered_map<std::string,FuncType> functions;
    }
}