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

    // Argument error in function
    class ArgumentError: public Error {
            std::string function_name;
            int line;
            int column;
            std::string comment="";

        public:
            ArgumentError(std::string function_name, int line, int column, std::string comment=""); // Constructure
            std::string too_many_arguments(int expected, int placed); // When the user places more arguments than expacted
            std::string unexpected_argument(std::string arg_name); // When user places unknown argument
            std::string unexpected_argument_type(std::string arg_name, std::string expected_type, std::string placed_type); // When user places flase type of function
            std::string unplaced_arg(std::string arg_name); // When user don't places all required arguments
    };
}