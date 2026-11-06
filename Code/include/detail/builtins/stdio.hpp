/**
 * @file stdio.hpp
 * @brief stdio declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "valtcommand/runtime.hpp"

/// == Libs ==
#include <functional>

/// Namespace
namespace Lib{
    
    /// All standard input output functions
    namespace STDIO {
        
        void __init__(Runtime::RunTime& runtime); ///< Add all functions in runtime

        /// Output ========
        ReturnResult<Value> print_fn(ExternalFunInType input); ///< print(a, b, ...) without newline
        ReturnResult<Value> println_fn(ExternalFunInType input); ///< println(a, b, ...) with newline

        /// Input ========
        ReturnResult<Value> input_fn(ExternalFunInType input); ///< input(prompt?) -> string
 
    }
}
