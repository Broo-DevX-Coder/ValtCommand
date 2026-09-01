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
#include <string>
#include <fmt/format.h>

// ==================================================================
// Namespace
// ==================================================================

// Namespace of All errors classes
namespace Errors {

    // Main Error class
    class Error {
        public:
            std::string msg; // Message of error
    };

    // Error in syntax of code
    class SyntaxError: public Error {
        public:
            SyntaxError(std::string token, int line, int column, std::string comment=""); // Constructure
    };

    // The type that got is not compatible with expected type
    class TypeError: public Error {
        public:
            TypeError(std::string expected, std::string got, int line, int column, std::string comment=""); // Constructure
    };

    // Name that declared not found
    class NameError: public Error {
        public:
            NameError(std::string name, int line, int column, std::string comment=""); // Constructure
    };
}