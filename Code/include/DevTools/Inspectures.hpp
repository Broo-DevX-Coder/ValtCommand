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

// == Locals ==
#include "Scope.hpp"

// == Libs ==
#include <nlohmann/json.hpp>

// ==================================================================
// Namespace
// ==================================================================

namespace DevTools {

    // Inspect return
    struct InspecturesReturn {
        std::string printable;
        nlohmann::json JSON;
    };

    // Get the basic information of the curent scope
    InspecturesReturn ScopeInspect(Scopes::Scope* scope);

    // Get visible variables table in entred scope
    InspecturesReturn VariablesInspect(Scopes::Scope* scope);

    // Get visible functions in entred scope
    InspecturesReturn FunctionsInspect(Scopes::Scope* scope);
}