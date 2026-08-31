// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Standards/init.hpp"

// standard functions
#include "Standards/Functions/print.hpp"

// ==================================================================
// Initialyze function
// ==================================================================

void
Standardes::__init__() {

    // Add functions to registry
    Runtime::registries::functions["print"] = Standardes::Functions::print;
}