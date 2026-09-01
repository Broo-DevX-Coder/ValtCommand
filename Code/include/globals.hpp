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
#include <fmt/format.h>

// ==================================================================
// Types 
// ==================================================================
using Value = std::variant<
    std::monostate,
    uint64_t, 
    double, 
    std::string, 
    bool
>; // Value variant type

// ==================================================================
// Enums
// ==================================================================

// tokenTypes enum
enum class TokenType {
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
// Vars 
// ==================================================================
extern std::unordered_map<TokenType,std::string> TokenTypesStr; // All TokenTypes like string

// ==================================================================
// Structs
// ==================================================================
struct Token {
    TokenType Type;
    std::string value;
    size_t line;
    size_t column;
    void print() {
        std::cout << fmt::format(
            "[ T:{} | V:{} | L:{} | C:{} ]",
            TokenTypesStr[Type], value, line, column
        ) << std::endl << std::flush;
    }
};

// ==================================================================
// Vars 
// ==================================================================
extern std::unordered_map<TokenType,std::string> TokenTypes_to_StringType; // All TokenTypes like string
extern std::vector<std::string> __types__; // All sepported types
extern std::vector<std::string> __key_words__; // All seported key words
extern std::unordered_map<char,Token> __symbols__; // All sepported symbols like <>:

// ==================================================================
// Functions
// ==================================================================
bool is_token_type_(std::string token);  // Is the token a type
bool is_token_key_word_(std::string token); // Is the token a keyword
std::string Get_ValueT(const Value& value); // Get Value type (what inside variant)

// Return object, to handle errors
template<typename T>
struct ReturnResult {
    std::string Message;
    bool success;
    T value;
};