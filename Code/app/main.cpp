// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <iostream>
#include <cctype>
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

// == Locals ==
#include "Runtime.hpp"
#include "Lexar.hpp"
#include "Errors.hpp"

std::ostream& operator<<(std::ostream& os, const std::monostate&) {
    os << "null";
    return os;
}

ReturnResult<Value> print(ExternalFunInType inputs) {
    for (auto& [n,i]: inputs) {

        if (std::holds_alternative<bool>(i)) {
            bool vv = false;
            bool* v = std::get_if<bool>(&i);

            if (v != nullptr) {
                vv = *v;
            }

            if (vv) std::cout << "True" << std::endl << std::flush;
            else std::cout << "False" << std::endl << std::flush;

            continue;
        }
            
        std::visit([](auto&& v){
            std::cout << v << std::endl << std::flush;
        }, i);
    }

    return {"",true,std::monostate{}};
}

ReturnResult<Value> Pi(ExternalFunInType inputs) {
    return {"",true,-3.14159265359};
}

// ==================================================================
// Entry point function
// ==================================================================
int main () {

    // ====== initialyze standards ==========
    //Standardes::__init__();

    std::cout << "======== Start exec =========" << "\n" << std::flush;


    std::string code = R"CODE(
    CALL print sa<str>:"hellow" END
    )CODE";
    
    auto r = Runtime::RunTime(code);
    r.add_external_function(print,"print","void",{},true);
    r.add_external_function(Pi,"Pi","float",{});

    auto execute = r.execute_code();
    if (!execute.success){
        std::cout << Errors::formatter(execute.error_info,code) << "\n\n" << std::flush;
        return 1;
    }
    std::cout << std::endl << "======== exec end =========" << std::endl << std::flush;

    return 0;
}