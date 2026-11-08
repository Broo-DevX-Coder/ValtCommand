/**
 * @file inspect.hpp
 * @brief inspect declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "valtcommand/scope.hpp"

/// == Libs ==
#include <nlohmann/json.hpp>

/// Namespace

namespace vltc {

namespace DevTools {

    /// Inspect return
    struct InspecturesReturn {
        std::string printable;
        nlohmann::json JSON;
    };

    /// Get the basic information of the curent scope
    InspecturesReturn ScopeInspect(Scopes::Scope* scope);

    /// Get visible variables table in entred scope
    InspecturesReturn VariablesInspect(Scopes::Scope* scope);

    /// Get visible functions in entred scope
    InspecturesReturn FunctionsInspect(Scopes::Scope* scope);
}

} // namespace vltc
