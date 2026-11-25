/**
 * @file inspect.hpp
 * @brief inspect declarations.
 */

#pragma once

#include "valtcommand/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "valtcommand/scope.hpp"

/// Namespace

namespace vltc {

namespace DevTools {

    /// Inspect return
    struct VALTC_API InspecturesReturn {
        std::string printable;
        std::string JSON;
    };

    /// Get the basic information of the curent scope
    VALTC_API InspecturesReturn ScopeInspect(Scopes::Scope* scope);

    /// Get visible variables table in entred scope
    VALTC_API InspecturesReturn VariablesInspect(Scopes::Scope* scope);

    /// Get visible functions in entred scope
    VALTC_API InspecturesReturn FunctionsInspect(Scopes::Scope* scope);
}

} // namespace vltc
