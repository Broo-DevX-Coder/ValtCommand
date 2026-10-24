
#pragma once

/**
 * @file globals.hpp
 * @brief Global types, structures, enums, and declarations for ValtCommand.
 *
 * This file contains the shared definitions used throughout the ValtCommand
 * interpreter, including:
 * - API export/import macros.
 * - Error information.
 * - Runtime values and execution states.
 * - Function execution results.
 * - External function interfaces.
 * - Token types and token structures.
 * - Global token definitions.
 * - Type compatibility and conversion functions.
 */

// ==================================================================
// Macros
// ==================================================================

/**
 * @brief Defines the API visibility for the compiler library.
 *
 * When building the compiler DLL, this macro exports symbols.
 * When using the compiler DLL, it imports symbols on Windows.
 *
 * On non-Windows platforms, symbol visibility is configured accordingly.
 */
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
// Include necessary headers
// ==================================================================

/**
 * @brief Standard and third-party dependencies required by this file.
 */

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <fmt/format.h>

// ==================================================================
// Forward declarations
// ==================================================================

/**
 * @brief Base AST node.
 */
class ASTNode;

/**
 * @brief Error-related declarations.
 */
namespace Errors {
    struct ErrorInfo;
}

/**
 * @brief Scope-related declarations.
 */
namespace Scopes {
    class Scope;
}

// ==================================================================
// Types
// ==================================================================

/**
 * @namespace Errors
 * @brief Contains error-related structures and definitions.
 */
namespace Errors {

    /**
     * @struct ErrorInfo
     * @brief Contains detailed information about a runtime or interpreter error.
     *
     * Stores the error type, message, additional information, and the
     * source location where the error occurred.
     */
    struct ErrorInfo {

        /**
         * @brief Error category or type.
         */
        std::string type;

        /**
         * @brief Main error message.
         */
        std::string message;

        /**
         * @brief Additional information about the error.
         */
        std::string comment;

        /**
         * @brief Source line where the error occurred.
         */
        std::size_t line = 0;

        /**
         * @brief Source column where the error occurred.
         */
        std::size_t column = 0;

        /**
         * @brief Length of the source region associated with the error.
         */
        std::size_t length = 1;
    };
}

/**
 * @brief Represents a runtime value supported by ValtCommand.
 *
 * The value can contain:
 * - std::monostate: Represents an empty or null value.
 * - int64_t: Represents an integer.
 * - long double: Represents a floating-point number.
 * - std::string: Represents a string.
 * - bool: Represents a boolean.
 */
using Value = std::variant<
    std::monostate,
    int64_t,
    long double,
    std::string,
    bool
>;

/**
 * @enum ExecState
 * @brief Represents the execution state of an AST node.
 *
 * Used to control interpreter execution and propagate control-flow
 * instructions between AST nodes.
 */
enum class ExecState {

    /**
     * @brief Normal execution.
     */
    Normal,

    /**
     * @brief Function return requested.
     */
    Return,

    /**
     * @brief Loop termination requested.
     */
    Break,

    /**
     * @brief Skip the current loop iteration.
     */
    Continue
};

/**
 * @struct ReturnResult
 * @brief Represents the result of executing an AST node or function.
 *
 * Contains the execution status, returned value, error information,
 * and additional information required to propagate function returns.
 *
 * @tparam T Type of the returned value.
 */
template<typename T>
struct ReturnResult {

    /**
     * @brief Message associated with the execution result.
     */
    std::string Message;

    /**
     * @brief Indicates whether execution succeeded.
     */
    bool success;

    /**
     * @brief Returned value.
     */
    T value;

    /**
     * @brief Execution state.
     *
     * Defaults to normal execution.
     */
    ExecState state = ExecState::Normal;

    /**
     * @brief AST nodes associated with function return operations.
     *
     * Used to identify the AST node that returned the data.
     */
    std::vector<ASTNode*> return_nodes;

    /**
     * @brief Value returned by the function.
     */
    Value return_data;

    /**
     * @brief Detailed error information.
     *
     * Contains the error type, message, source location,
     * and additional information.
     */
    Errors::ErrorInfo error_info;
};

// ==================================================================
// External function types
// ==================================================================

/**
 * @struct ExternalFunInType
 * @brief Represents the input data provided to an external C++ function.
 *
 * Contains the function arguments and the parent scope from which
 * the external function was called.
 */
struct ExternalFunInType {

    /**
     * @brief Type used to represent external function arguments.
     *
     * Maps argument names to their runtime values.
     */
    using ArgsType = std::unordered_map<std::string, Value>;

    /**
     * @brief Arguments passed to the external function.
     */
    ArgsType arguments;

    /**
     * @brief Parent scope of the external function.
     *
     * Provides access to the scope from which the function was called.
     */
    Scopes::Scope* ParentScope;
};

/**
 * @brief Represents the signature of an external C++ function.
 *
 * External functions receive ExternalFunInType as input and return
 * a ReturnResult containing a ValtCommand runtime value.
 */
using ExternalFuncType =
    std::function<ReturnResult<Value>(ExternalFunInType)>;

// ==================================================================
// Enums
// ==================================================================

/**
 * @enum TokenType
 * @brief Represents the different token types recognized by the lexer.
 *
 * Includes identifiers, literals, keywords, operators, punctuation,
 * and special tokens used during lexical analysis and parsing.
 */
enum class TokenType {

    /** @brief Identifier token. */
    IDENTIFIER,

    /** @brief Data type token. */
    TYPE,

    /** @brief Language keyword token. */
    KEY_WORD,

    /** @brief String literal token. */
    STRING,

    /** @brief Integer literal token. */
    INTEGER,

    /** @brief Floating-point literal token. */
    FLOAT,

    /** @brief Boolean literal token. */
    BOOLEAN,

    /** @brief Colon symbol. */
    COLON,

    /** @brief Less-than operator (<). */
    LESS_THAN,

    /** @brief Greater-than operator (>). */
    GREATER_THAN,

    /** @brief Left square bracket ([). */
    LEFT_BRACKET,

    /** @brief Right square bracket (]). */
    RIGHT_BRACKET,

    /** @brief Left brace ({). */
    LEFT_BRACE,

    /** @brief Right brace (}). */
    RIGHT_BRACE,

    /** @brief Left parenthesis ((). */
    LEFT_PAREN,

    /** @brief Right parenthesis ()). */
    RIGHT_PAREN,

    /** @brief Slash operator (/). */
    SLASH,

    /** @brief Backslash symbol (\). */
    BACKSLASH,

    /** @brief Semicolon symbol (;). */
    SEMICOLON,

    /** @brief Comma symbol (,). */
    COMMA,

    /** @brief Multiplication operator (*). */
    STAR,

    /** @brief Subtraction operator (-). */
    MINUS,

    /** @brief Addition operator (+). */
    PLUS,

    /** @brief Assignment operator (=). */
    EQUAL,

    /** @brief Caret symbol (^). */
    CARET,

    /** @brief End-of-block token. */
    END_BLOCK,

    /** @brief End-of-code token. */
    END_CODE,

    /** @brief Exclamation mark (!). */
    EXCLAMATION,

    /** @brief Equality operator (==). */
    EQUAL_EQUAL,

    /** @brief Inequality operator (!=). */
    NOT_EQUAL,

    /** @brief Less-than-or-equal operator (<=). */
    LESS_EQUAL,

    /** @brief Greater-than-or-equal operator (>=). */
    GREATER_EQUAL,

    /** @brief Logical AND operator (&&). */
    LOGICAL_AND,

    /** @brief Logical OR operator (||). */
    LOGICAL_OR,

    /** @brief Power operator. */
    POWER,

    /** @brief Integer division operator (div). */
    DIV,

    /** @brief Modulo operator (mod). */
    MOD,

    /** @brief Unknown or unsupported token. */
    UNKNOWN
};

// ==================================================================
// Global variables
// ==================================================================

/**
 * @brief Maps token types to their corresponding string representations.
 *
 * Used to convert TokenType values into readable strings.
 */
extern std::unordered_map<TokenType, std::string> TokenTypesStr;

// ==================================================================
// Structures
// ==================================================================

/**
 * @struct Token
 * @brief Represents a lexical token generated by the lexer.
 *
 * Contains the token type, its textual value, and its source location.
 */
struct Token {

    /**
     * @brief Type of the token.
     */
    TokenType Type;

    /**
     * @brief Textual value of the token.
     */
    std::string value;

    /**
     * @brief Source line where the token begins.
     */
    size_t line;

    /**
     * @brief Starting column of the token.
     */
    size_t column;

    /**
     * @brief Ending column of the token.
     */
    size_t last_column;

    /**
     * @brief Prints the token information to standard output.
     *
     * Displays the token type, value, line, and column.
     */
    void print() {

        std::cout << fmt::format(
            "[ T:{} | V:{} | L:{} | C:{} ]",
            TokenTypesStr[Type],
            value,
            line,
            column
        ) << std::endl << std::flush;
    }
};

// ==================================================================
// Global token definitions
// ==================================================================

/**
 * @brief Maps token types to their string representations.
 *
 * Used to retrieve the readable representation of a token type.
 */
extern std::unordered_map<TokenType, std::string>
    TokenTypes_to_StringType;

/**
 * @brief Contains all supported ValtCommand data types.
 */
extern std::vector<std::string> __types__;

/**
 * @brief Contains all supported ValtCommand keywords.
 */
extern std::vector<std::string> __key_words__;

/**
 * @brief Maps supported single-character symbols to their tokens.
 */
extern std::unordered_map<char, Token> __symbols__;

/**
 * @brief Maps supported multi-character symbols to their tokens.
 *
 * Examples include equality and logical operators.
 */
extern std::unordered_map<std::string, Token> __complex_2_symbols__;

/**
 * @brief Maps escaped characters to their corresponding characters.
 *
 * Examples include newline and tab escape sequences.
 */
extern std::unordered_map<char, char> __backslashed_symbols__;

// ==================================================================
// Functions
// ==================================================================

/**
 * @brief Checks whether a string represents a supported data type.
 *
 * @param token String to check.
 * @return true if the string represents a supported type, otherwise false.
 */
bool is_token_type_(std::string token);

/**
 * @brief Checks whether a string represents a supported keyword.
 *
 * @param token String to check.
 * @return true if the string represents a supported keyword, otherwise false.
 */
bool is_token_key_word_(std::string token);

/**
 * @brief Retrieves the type name of a runtime value.
 *
 * Determines the active alternative of the Value variant and returns
 * its corresponding ValtCommand type name.
 *
 * @param value Runtime value to inspect.
 * @return String representing the runtime value type.
 */
std::string Get_ValueT(const Value& value);

/**
 * @brief Checks whether two ValtCommand data types are compatible.
 *
 * Determines whether values of the first type can be used with
 * values of the second type.
 *
 * @param first First data type.
 * @param secound Second data type.
 * @return true if the types are compatible, otherwise false.
 */
bool are_types_compatible(
    const std::string& first,
    const std::string& secound
);

/**
 * @brief Reconciles an integer or floating-point value with a target type.
 *
 * Converts the input value to the requested numeric type when
 * the conversion is supported.
 *
 * @param type Target data type.
 * @param input Value to convert.
 * @return Converted runtime value.
 */
Value reconsiliation_int_float(
    const std::string& type,
    Value& input
);

/**
 * @brief Converts a runtime value to a floating-point value.
 *
 * Attempts to convert the provided Value into a long double.
 *
 * @param input Runtime value to convert.
 * @return ReturnResult containing the converted floating-point value
 *         or error information if conversion fails.
 */
ReturnResult<long double> turn_value_to_float(Value& input);
