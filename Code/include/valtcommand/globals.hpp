
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


#include "valtcommand/api_export.hpp"
#include "valtcommand/result.hpp"
#include "valtcommand/value.hpp"

/// Include necessary headers

/**
 * @brief Standard and third-party dependencies required by this file.
 */

#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

/// Forward declarations

/**
 * @brief Base AST node.
 */
namespace vltc {

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

/// Types

/**
 * @enum ExecState
 * @brief Represents the execution state of an AST node.
 *
 * Used to control interpreter execution and propagate control-flow
 * instructions between AST nodes.
 */
enum class VALTC_API ExecState {

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
struct VALTC_API ReturnResult {

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

/// External function types

/**
 * @struct ExternalFunInType
 * @brief Represents the input data provided to an external C++ function.
 *
 * Contains the function arguments and the parent scope from which
 * the external function was called.
 */
struct VALTC_API ExternalFunInType {

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

/// Enums

/**
 * @enum TokenType
 * @brief Represents the different token types recognized by the lexer.
 *
 * Includes identifiers, literals, keywords, operators, punctuation,
 * and special tokens used during lexical analysis and parsing.
 */
enum class VALTC_API TokenType {

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
    UNKNOWN,

    /** @brief Unterminated string literal. */
    UNTERMINATED_STRING,

    /** @brief Unterminated block comment. */
    UNTERMINATED_COMMENT
};

/// Structures

/**
 * @struct Token
 * @brief Represents a lexical token generated by the lexer.
 *
 * Contains the token type, its textual value, and its source location.
 */
struct VALTC_API Token {

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
     * @brief The FilePath of where the token founded
     */
    fs::path FilePath;

    /** @brief Prints the token information to standard output. */
    void print();
};


} // namespace vltc
