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
    )->bool 

        // Verifi if input is less then 10
        IF GET arg < 10 THEN
            RETURN True
            CALL print va<int>:45155146 END
        END

        // Returning false if the input is bigest then 10
        RETURN False

    END

    SET_CONST var<float> = 11.56

    CALL print ca<str>:"-------------------------------------" END
    CALL print 
        va<bool>:CALL my_func arg<int>:11 END
        vaa<str>:"11 < 10 is:"
    END
    CALL print ca<str>:"-------------------------------------" END
    CALL print 
        va<bool>:CALL my_func arg<int>:9 END
        vaa<str>:"9 < 10 is:"
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