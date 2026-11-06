/**
 * @file result.hpp
 * @brief Public source-location details attached to runtime errors.
 */
#pragma once

#include <cstddef>
#include <string>

/**
 * @namespace Errors
 * @brief Error data shared by the runtime and its host.
 */
namespace Errors {

/**
 * @struct ErrorInfo
 * @brief Describes an error and its location in source code.
 */
struct ErrorInfo {
    std::string type;     ///< Error category.
    std::string message;  ///< Main error message.
    std::string comment;  ///< Additional diagnostic detail.
    std::size_t line = 0; ///< Source line, or zero when unknown.
    std::size_t column = 0; ///< Source column, or zero when unknown.
    std::size_t length = 1; ///< Length of the source region.
};

}

namespace Runtime {

    /**
     * @brief The stage of the pipeline that produced an error.
     */
    enum class ErrorStage {
        None, ///< No error.
        IO, ///< RunTime::run_file(): the file is missing or unreadable.
        Parse, ///< Lexer or parser failed.
        Semantic, ///< Semantic analysis failed.
        Runtime, ///< Execution failed.
        Host ///< Host API misuse (unknown variable, invalid type, ...).
    };

    /**
     * @brief Details of a failed operation.
     */
    struct RunError {
        ErrorStage stage = ErrorStage::None; ///< Where the error happened.
        std::string type; ///< Error class of the language ("NameError", "TypeError", ...), "" if none.
        std::string message; ///< Human-readable error message.
        int line = 0; ///< Line, numbered like Errors::ErrorInfo (0 = unknown).
        int column = 0; ///< Column, numbered like Errors::ErrorInfo (0 = unknown).
        Errors::ErrorInfo info; ///< All info used in formatter
    };

    /**
     * @brief Result of RunTime::run(), RunTime::run_file(),
     *        RunTime::set_variable() and RunTime::get_variable().
     *
     * Failures are reported here instead of being thrown, so a bad script can
     * never crash the host. The result converts to @c bool, so
     * @code{.cpp}
     * if (!result) { ... }
     * @endcode
     * works as a failure check.
     */
    struct RunResult {
        bool ok = true; ///< @c true if the operation succeeded.
        std::string value; ///< Textual form of the last value ("" if none).
        std::string type; ///< Type name of that value ("" if none).
        RunError error; ///< Valid only when @ref ok is @c false.

        /// @return The same as @ref ok.
        explicit operator bool() const { return ok; }
    };

    /**
     * @brief How a call to RunTime::run() treats the global scope.
     */
    enum class ScopeMode {
        Persistent, ///< Reuse the global scope: variables and functions survive between calls.
        Fresh ///< Run in a temporary scope; the persistent state is untouched.
    };


}
