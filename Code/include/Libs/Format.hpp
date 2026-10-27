#pragma once

// ==================================================================
// Macros
// ==================================================================

/**
 * @def API
 * @brief Controls symbol visibility for the ValtCommand shared library.
 *
 * This macro determines whether a function or class is exported from
 * the ValtCommand shared library or imported by a consuming application.
 *
 * Behavior:
 * - When BUILDING_COMPILER_DLL is defined:
 *   - Windows: expands to __declspec(dllexport).
 *   - Other platforms: expands to __attribute__((visibility("default"))).
 *
 * - When BUILDING_COMPILER_DLL is not defined:
 *   - Windows: expands to __declspec(dllimport).
 *   - Other platforms: expands to nothing.
 *
 * @note Define BUILDING_COMPILER_DLL when compiling the ValtCommand
 *       shared library itself.
 *
 * @note On non-Windows platforms, the macro explicitly exports symbols
 *       when building the library and has no effect for consumers.
 */
#ifndef API
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
#endif

// ==================================================================
// Includes
// ==================================================================

#include "API/Runtime.hpp"

// ==================================================================
// Namespace: Lib::Format
// ==================================================================

namespace Lib {

    /**
     * @namespace Format
     * @brief Provides built-in string formatting functionality.
     *
     * This namespace contains functions that can be registered in a
     * ValtCommand runtime and called directly from ValtCommand code.
     *
     * The formatting API supports named placeholders in the form
     * @c {name}. Each placeholder is replaced with the string
     * representation of the corresponding named argument.
     *
     * Example:
     * @code
     * CALL format
     *     text<str>:"Hello {name}!"
     *     name<str>:"World"
     * END
     * @endcode
     *
     * Expected result:
     * @code
     * Hello World!
     * @endcode
     *
     * @note The @c text argument is mandatory. Other named arguments are
     *       provided dynamically and are not declared individually in
     *       the function registration.
     *
     * @warning A runtime error is returned if @c text is missing, a
     *          placeholder is malformed, or a placeholder references
     *          an argument that was not supplied.
     */
    namespace Format {

        /**
         * @brief Registers the formatting functions in a runtime.
         *
         * Registers @c format as a native ValtCommand function. The
         * function accepts dynamically named arguments, allowing callers
         * to provide any number of values for placeholder substitution.
         *
         * The arguments are validated by @ref format_fn at runtime.
         *
         * @param runtime The runtime in which the formatting functions
         *                will be registered.
         *
         * @note Call this function during runtime initialization before
         *       executing ValtCommand code that uses @c format.
         */
        API void __init__(Runtime::RunTime& runtime);

        /**
         * @brief Formats a string by replacing named placeholders.
         *
         * Reads the required @c text argument and replaces each
         * placeholder of the form @c {name} with the string
         * representation of the corresponding value in
         * @c input.arguments.
         *
         * Supported value types are determined by the ValtCommand
         * @c Value variant, including integers, floating-point values,
         * strings, and booleans.
         *
         * For example, given the following named arguments:
         *
         * @code
         * text = "Hello {name}, age: {age}"
         * name = "World"
         * age  = 18
         * @endcode
         *
         * The resulting string is:
         *
         * @code
         * Hello World, age: 18
         * @endcode
         *
         * @param input The named arguments supplied by ValtCommand.
         *              The @c text argument contains the format string;
         *              all other arguments are available for placeholder
         *              substitution.
         *
         * @return A successful @c ReturnResult<Value> containing the
         *         formatted string on success.
         *
         * @retval ReturnResult<Value> An unsuccessful result containing
         *         an appropriate runtime error when:
         *         - The required @c text argument is missing.
         *         - The @c text argument is not a string.
         *         - A placeholder is empty or has no closing brace.
         *         - A placeholder references a missing named argument.
         */
        API ReturnResult<Value> format_fn(ExternalFunInType input);

    } // namespace Format

} // namespace Lib