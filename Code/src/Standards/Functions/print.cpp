// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Standards/Functions/print.hpp"

// ==================================================================
// Function
// ==================================================================
ReturnResult<Value>
Standardes::Functions::print(
    Runtime::FunInType in_
) {
    for (const auto& [n_,input]: in_) {
        std::cout << n_ << "\n" << std::flush;
    }
    return {"",true,std::monostate()};
}
/*
for (int i = in_.size()-1;i>=0;i--) {
        std::cout << in_ << "\n" << std::flush;
    }
*/