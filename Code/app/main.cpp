// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <iostream>
#include <variant>
#include <string>

// == Locals ==
#include "API/Runtime.hpp"
#include "Errors.hpp"
#include "Libs/DevTools.hpp"

std::ostream& operator<<(std::ostream& os, const std::monostate&) {
    os << "null";
    return os;
}

ReturnResult<Value> print(ExternalFunInType input) {
    for (auto& [n,i]: input.arguments) {

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

ReturnResult<Value> Pi(ExternalFunInType input) {
    return {"",true,-3.14159265359L};
}

// ==================================================================
// Entry point function
// ==================================================================
int main () {

    // ====== initialyze standards ==========
    //Standardes::__init__();

    std::cout << "======== Start exec =========" << "\n" << std::flush;


    std::string code = R"CODE(

    SET var<int> = 5
    CALL print value<int>:(GET var) END

    )CODE";
    
    {
        auto r = Runtime::RunTime();
        r.add_external_function(print,"print","void",{},true);
        r.add_external_function(Pi,"Pi","float",{},true);
        Lib::DevFunctions::__init__(r);
    
        auto execute = r.run(code,Runtime::ScopeMode::Persistent);
        if (!execute){
            std::cout << Errors::formatter(execute.error.info,code) << "\n\n" << std::flush;
            return 1;
        }
    }
    std::cout << std::endl << "======== exec end =========" << std::endl << std::flush;

    return 0;
}