/**
 * @file devtools.hpp
 * @brief devtools declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "valtcommand/runtime.hpp"

/// == Libs ==
#include <functional>

/// Namespace
namespace vltc {

namespace Lib{
    
    /// All devtools embeded functions
    namespace DevFunctions {
        
        void __init__(Runtime::RunTime& runtime); ///< Add all functions in runtime

        /// All Inspectors ========
        ReturnResult<Value> scope_inspect(ExternalFunInType input); ///< Inspect scope
        ReturnResult<Value> vars_inspect(ExternalFunInType input); ///< Inspect vars
        ReturnResult<Value> funcs_inspect(ExternalFunInType input); ///< Inspect functions
 
    }
}

} // namespace vltc
