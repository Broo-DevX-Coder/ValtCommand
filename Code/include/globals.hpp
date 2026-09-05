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
bool are_types_compatible(const std::string& first, const std::string& secound); // Are two types compatible (like int with float)

// ==================================================================
// Scopes space
// ==================================================================
namespace Scopes {

    namespace registries {
        API extern std::unordered_map<std::string,ExternalFuncType> functions;
    }

    namespace SymbolTableTypes {
        enum class FunctionsTypes {
            Extenal, // An external function comes from pure c++
            Inside // The user function `SETFUNCTION`
        };

        // Variable in scope
        struct SVar {
            std::string type; // Type of data in var
            bool is_const; // Is the var const `can't change value`
        };

        // Variable with value in scope
        struct RVar: public SVar {
            Value value; // The value of var
            size_t scope_id; // The scope id where the var created
        };

        // Function's method in scope
        struct Method: public SVar {
            bool is_required; // Is the method required 
            bool is_any;  // Is the type of method any (means the method can be any type)
        };
        
        // Function in scope
        struct Function {
            std::string return_type; // The type of value returned
            std::unordered_map<std::string, Method> methods; // All methods of function
            bool is_sepport_any_methods_=false; // Is function can get any methods by any types (like print)
            size_t scope_id; // The id of scope in which this function was defined
            FunctionsTypes type; // The type of function (inside or external)
            ExternalFuncType external_func; // The pure c++ function if type is exernal
            /* A place for ASTNode function */  // A copy of Node of executing function if the type is inside
        };

    }

    // Scope
    class Scope {
        protected:
            Scope* Parent; // The parent scope of this scope
            size_t ID; // The id of scope
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::Function>> functions; // All functions in scope
            std::unordered_map<std::string, std::unique_ptr<SymbolTableTypes::RVar>> variables; // All variables in scope

        public:
            Scope(Scope* parent = nullptr); // constructure
            size_t get_id(); // Get the ID of scope
            SymbolTableTypes::Function* add_function(const std::string& name, const std::string& return_type, std::unordered_map<std::string, SymbolTableTypes::Method> methods, bool is_any=false); // add function to scope table
            SymbolTableTypes::RVar* add_var(const std::string& name, const std::string& type, Value& value, bool is_const=false); // add variable to scope table
            ReturnResult<SymbolTableTypes::Function*> search_function(Token& NameToken); // Get a function struct pointer from scope by name
            ReturnResult<SymbolTableTypes::RVar*> search_var(Token& NameToken); // Get a variable struct pointer from scope by name
    };
}