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
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <fmt/format.h>

// ==================================================================
// Forwarding declarations
// ==================================================================
class ASTNode;

// ==================================================================
// Types 
// ==================================================================
using Value = std::variant<
    std::monostate,
    int64_t, 
    long double, 
    std::string, 
    bool
>; // Value variant type

enum class ExecState {
    Normal,
    Return,
    Break,
    Continue
};

// Return object, to handle errors
template<typename T>
struct ReturnResult {
    std::string Message;
    bool success;
    T value;
    ExecState state = ExecState::Normal;

    std::vector<ASTNode*> return_nodes; // For function return: whow is the node that returns data
    Value return_data; // For function return: what did the function return
};

using ExternalFunInType = std::unordered_map<std::string, Value>; // External functin input type
using ExternalFuncType = std::function<ReturnResult<Value>(ExternalFunInType)>; // External function type

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
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    SLASH,
    BACKSLASH,
    STAR,
    MINUS,
    PLUS,
    EQUAL,
    CARET,
    END_BLOCK,
    END_CODE,
    EXCLAMATION,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    UNKNOWN
};

// ==================================================================
// Vars 
// ==================================================================
extern std::unordered_map<TokenType,std::string> TokenTypesStr; // All TokenTypes like string

// ==================================================================
// Structs
// ==================================================================

// Token in code
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
extern std::unordered_map<std::string, Token> __complex_2_symbols__; // All Complex symbols like `==` and `||` ()
extern std::unordered_map<char,char> __backslashed_symbols__; // All symbols like `\n` or `\t`

// ==================================================================
// Functions
// ==================================================================
bool is_token_type_(std::string token);  // Is the token a type
bool is_token_key_word_(std::string token); // Is the token a keyword
std::string Get_ValueT(const Value& value); // Get Value type (what inside variant)
bool are_types_compatible(const std::string& first, const std::string& secound); // Are two types compatible (like int with float)
Value reconsiliation_int_float(const std::string& type, Value& input); // Reconsiliation between intiger and float
ReturnResult<long double> turn_value_to_float(Value& input); // Turn a Value type to float