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
#include <fmt/format.h>

// == Locals ==
#include "API/globals.hpp"

// ==================================================================
// Namespace
// ==================================================================

// Namespace of All errors classes
namespace Errors {

    // Errors formatter
    std::string formatter(ErrorInfo info, const std::string& code);

    // Main Error class
    class Error {
        public:
            std::string msg; // Message of error
            ErrorInfo info; // Error info struct
    };

    // Error in syntax of code
    class SyntaxError: public Error {
        public:
            SyntaxError(std::string token, size_t line, size_t column, size_t lenth, std::string comment=""); // Constructure
    };

    // The type that got is not compatible with expected type
    class TypeError: public Error {
        public:
            TypeError(std::string expected, std::string got, size_t line, size_t column, size_t lenth, std::string comment=""); // Constructure
    };

    // Name that declared not found
    class NameError: public Error {
        public:
            NameError(std::string name, size_t line, size_t column, size_t lenth, std::string comment=""); // Constructure
    };

    // Argument error in function
    class ArgumentError: public Error {
        private:
            std::string function_name;

        public:
            ArgumentError(std::string function_name, size_t line, size_t column, size_t lenth, std::string comment=""); // Constructure
            std::string too_many_arguments(size_t expected, size_t placed); // When the user places more arguments than expacted
            std::string unexpected_argument(std::string arg_name); // When user places unknown argument
            std::string unexpected_argument_type(std::string arg_name, std::string expected_type, std::string placed_type); // When user places flase type of function
            std::string unplaced_arg(std::string arg_name); // When user don't places all required arguments
    };

    // Variable Error
    class VariableError: public Error {
        private:
            std::string var_name;
        public:
            VariableError(std::string var_name, size_t line, size_t column, size_t lenth, std::string comment="");  // Constructure
            std::string reset_var_in_same_scope(); // When recreate variable in same scope
            std::string reset_const_var(); // When user reset a const variable
            std::string reset_var_value_by_other_type(std::string var_type, std::string got_type); // When reset variable value but new value is incompatible type with variable type

    };

    // User Function Error
    class UserFunctionError : public Error {
        private:
            std::string function_name;

        public:
            UserFunctionError(std::string function_name,size_t line,size_t column, size_t lenth,std::string comment = ""); // Constructure
            std::string return_type_error(std::string expected_type,std::string got_type); // When user returns uncompatible type data with function return type
            std::string missing_return_error(); // Not all paths return a value
            std::string function_already_defined(); // When user redifine a function by same name in same scope
    };

    // Runtime Error
    class RuntimeError : public Error {
        public:
            RuntimeError(
                size_t line,
                size_t column, 
                size_t lenth,
                std::string comment = ""
            );

            std::string division_by_zero(); // When user devide on zero
    };
}