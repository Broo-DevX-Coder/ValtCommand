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
// Types 
// ==================================================================
using Value = std::variant<
    std::monostate,
    uint64_t, 
    double, 
    std::string, 
    bool
>; // Value variant type

// Return object, to handle errors
template<typename T>
struct ReturnResult {
    std::string Message;
    bool success;
    T value;
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

// ==================================================================
// Scopes space
// ==================================================================
namespace Scopes {

    namespace registries {
        API extern std::unordered_map<std::string,ExternalFuncType> functions;
    }

    namespace SymbolTableTypes {
        enum class FunctionsTypes {
            Extenal,
            Inside
        };

        // Variable in scope
        struct SVar {
            std::string type;
            bool is_const;
        };

        // Variable with value in scope
        struct RVar: public SVar {
            Value value;
        };

        // Function's method in scope
        struct Method: public SVar {};
        
        // Function in scope
        struct Function {
            std::string return_type;
            std::unordered_map<std::string, Method> methods;
            FunctionsTypes type;
            ExternalFuncType external_func;
        };

    }

    // Scope
    class Scope {
        protected:
            Scope* Parent;
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::Function>> functions; // All functions in scope
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::RVar>> variables; // All variables in scope

        public:
            Scope(Scope* parent = nullptr); // constructure
            SymbolTableTypes::Function* add_function(const std::string& name, const std::string& return_type, std::unordered_map<std::string, SymbolTableTypes::Method> methods); // add function to scope table
            SymbolTableTypes::RVar* add_var(const std::string& name, const std::string& type, Value& value, bool is_const=false); // add variable to scope table
            ReturnResult<SymbolTableTypes::Function*> search_function(Token& NameToken); // Get a function struct pointer from scope by name
            ReturnResult<SymbolTableTypes::RVar*> search_var(Token& NameToken); // Get a variable struct pointer from scope by name
    };
}