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
    // Title: Calculate number power number (x^n) sush that: n belong to R

// Create my function
FUNCTION power (
    num<float> // Number that we want
    power<int> // Power that we want to raise the numer
)->float 

    SET var<float> = 1 // Set the result in first 1
    SET pointer<int> = 0 // The pointer should be 0

    IF (GET power) == 0 THEN // If the power is 0, so the result will be 1
        RETURN 1
    ELSE THEN IF (GET num) == 0 THEN // If the number is 0, of cors the result is 0
        RETURN 0
    END END

    // Calculating operation 
    WHILE (GET pointer) < (GET power) THEN
        SET var = (GET var)*(GET num) 
        SET pointer = (GET pointer)+1
    END

    // Return reslt
    RETURN GET var

END
    
// Ex: Calculate 2 power 3 `should be 8`
CALL print 
    value<float>: CALL power 
        num<float>:2
        power<int>:3
    END 
END

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