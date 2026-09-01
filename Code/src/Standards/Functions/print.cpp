// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Standards/Functions/print.hpp"

// ==================================================================
// Initialyze operators
// ==================================================================
std::ostream& operator<<(
    std::ostream& os,
    const std::monostate&
) {
    return os << "null";
}

// ==================================================================
// Function
// ==================================================================
ReturnResult<Value>
Standardes::Functions::print(
    Runtime::FunInType in_
) {
    for (const auto& [n_,input]: in_) {
        std::visit([](auto&& v){
            std::cout << v << "\n" << std::flush;
        },input);
    }
    return {"",true,std::monostate()};
}
/*
for (int i = in_.size()-1;i>=0;i--) {
        std::cout << in_ << "\n" << std::flush;
    }
*/