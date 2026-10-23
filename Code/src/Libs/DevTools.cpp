// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Libs/DevTools.hpp" 
#include "API/DevTools/Inspectures.hpp"


// ==================================================================
// Function: Initailyze runtime
// ==================================================================

void
Lib::DevFunctions::__init__(
    Runtime::RunTime& runtime
) {
    // Add scope inspecture
    runtime.add_external_function(
        scope_inspect,
        "__INSPECT_SCOPE__",
        "str",{},false
    );

    // Add vars inspecture
    runtime.add_external_function(
        vars_inspect,
        "__INSPECT_VARS__",
        "str",{},false
    );

    // Add funcs inspecture
    runtime.add_external_function(
        funcs_inspect,
        "__INSPECT_FUNCS__",
        "str",{},false
    );
}


// ==================================================================
// Function: Inspect scope
// ==================================================================

ReturnResult<Value> 
Lib::DevFunctions::scope_inspect(
    ExternalFunInType input
) {
    if (input.ParentScope) {
        return {
            "",true,DevTools::ScopeInspect(input.ParentScope).printable
        };
    } 
    return {"",true,""};
}

// ==================================================================
// Function: Inspect variables
// ==================================================================

ReturnResult<Value> 
Lib::DevFunctions::vars_inspect(
    ExternalFunInType input
) {
    if (input.ParentScope) {
        return {
            "",true,DevTools::VariablesInspect(input.ParentScope).printable
        };
    } 
    return {"",true,""};
}

// ==================================================================
// Function: Inspect Functions
// ==================================================================

ReturnResult<Value> 
Lib::DevFunctions::funcs_inspect(
    ExternalFunInType input
) {
    if (input.ParentScope) {
        return {
            "",true,DevTools::FunctionsInspect(input.ParentScope).printable
        };
    } 
    return {"",true,""};
}