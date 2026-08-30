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
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

// ==================================================================
// Enums
// ==================================================================
enum TokenType {
    IDENTIFIER,
    TYPE,
    KEY_WORD,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    COLON,
    LESS_THAN,
    GREATER_THAN,
    NEWLINE,
    END_CODE,
    UNKNOWN
};

// ==================================================================
// Structs
// ==================================================================
struct API Token {
    TokenType Type;
    std::string value;
    void print() {
        std::cout << "{" << Type << " - " << value << "}" << std::endl << std::flush;
    }
};

// ==================================================================
// Vars
// ==================================================================
API extern std::vector<std::string> __types__; // All sepported types
API extern std::vector<std::string> __key_words__; // All seported key words
API extern std::unordered_map<char,Token> __symbols__; // All sepported symbols like <>:

// ==================================================================
// Types 
// ==================================================================
using Value = std::variant<int, double, std::string, bool>; // Value variant type


// ==================================================================
// Functions
// ==================================================================
API bool is_token_type_(std::string token);  // Is the token a type
API bool is_token_key_word_(std::string token); // Is the token a keyword