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

std::ostream& operator<<(std::ostream& os, const std::monostate&) {
    os << "null";
    return os;
}

ReturnResult<Value> print(ExternalFunInType inputs) {
    for (auto& [n,i]: inputs) {
        std::visit([](auto&& v){
            std::cout << v << std::endl << std::flush;
        }, i);
    }

    return {"",true,std::monostate{}};
}

ReturnResult<Value> Pi(ExternalFunInType inputs) {
    return {"",true,3.14159265359};
}

// ==================================================================
// Entry point function
// ==================================================================
int main () {

    // ====== initialyze standards ==========
    //Standardes::__init__();

    std::string code = R"CODE(CALL print 
        value__<void>: 
            CALL print
                value0<float>:CALL Pi v<str>:"boy" END
                value1<float>:CALL Pi v<str>:22.5 END
                value2<str>:"gg"
                value3<str>:"ee"
            END
    END
    )CODE";

    auto r = Runtime::RunTime(code);
    r.add_external_function(print,"print","void",{},true);
    r.add_external_function(Pi,"Pi","float",{});

    auto semantic_analyses = r.semantic_analyses();
    if (!semantic_analyses.success){
        std::cout << "======== Semantic error =========" << std::endl << std::flush;
        std::cout << semantic_analyses.Message << std::endl << std::flush;
        return 1;
    }
    std::cout << "======== Semantic end =========" << std::endl << std::flush;

    auto execute = r.execute_code();
    if (!execute.success){
        std::cout << "======== Runtime error =========" << std::endl << std::flush;
        std::cout << execute.Message << std::endl << std::flush;
        return 1;
    }
    std::cout << "======== exec end =========" << std::endl << std::flush;

    return 0;
}