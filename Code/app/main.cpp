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
    SET_CONST Pi<int> = 3.14
    
    // Create my function
    FUNCTION power (
        num<float> 
        power<int>
    )->float 

        SET var<int> = 1
        SET pointer<int> = 0

        IF (GET power) == 0 THEN
            RETURN 1

        ELSE THEN IF (GET num) == 0 THEN
            RETURN 0

        END END

        WHILE (GET pointer) < (GET power) THEN
            CALL print c<float>: GET pointer END
            SET var = (GET var)*(GET num) 
            SET pointer = (GET pointer)+1
            CONTINUE
            CALL print d<str>:"-----" END
        END

        RETURN GET var

    END
    
    // Calculate 2 power 3 `should be 8`
    CALL print 
        s<float>: CALL power 
            num<float>:5
            power<int>:8
        END 
        cc<str>:"-----------------"
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