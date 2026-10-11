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
#include <random>
#include <string>

// ==================================================================
// Namespace
// ==================================================================
namespace Utils {
    size_t random_num(size_t from = 0, size_t to=SIZE_MAX); // Create a random number
    std::vector<std::string> split_lines(const std::string& source); // Split string to vector of lines 
}