// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Standards/Functions/print.hpp"

// ==================================================================
// Function
// ==================================================================
void
Standardes::Functions::print(
    Runtime::FunInType in_
) {
    for (const auto& [n_,input]: in_) {
        std::cout << n_ << "\n" << std::flush;
    }
}