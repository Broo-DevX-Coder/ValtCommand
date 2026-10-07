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

    std::string code = R"CODE(
    
    // Create my function
    FUNCTION my_func (
        arg<int>
    )->float 

        SET var_arg<float> = (GET arg + 2*1788.55) / 888 + 9

        RETURN GET var_arg

    END

    SET_CONST var<float> = CALL Pi END

    CALL print ca<str>:"-------------------------------------" END
    CALL print 
        va<float>:CALL my_func arg<int>:100000 END
        vaa<str>:"float is:"
    END
    CALL print ca<str>:"-------------------------------------" END
    CALL print 
        va<int>:CALL my_func arg<int>:100000 END
        vaa<str>:"int is:"
    END

    SET var_arg<float> = 5/0

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