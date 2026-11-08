/**
 * @file native_function.hpp
 * @brief Types used to describe arguments to host-provided native functions.
 */
#pragma once

#include "valtcommand/value.hpp"

namespace vltc {

namespace Runtime {

    /**
     * @brief The struct of agrument that includes every thing about them
     */
    struct ExternalFunctionArgument {
        std::string type; ///< Type of data in var
        bool is_const; ///< Is the var const `can't change value`
        bool is_required; ///< Is the argument required
        bool is_any; ///< Is the type of argument any (means the argument can be any type)
        Value default_value; ///< The default alue of argument in set function (arg<str> = "hh")
    };


}

} // namespace vltc
