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
    
    FUNCTION multiple (
        num1<float>
        num2<float> =5
    )->float

        CALL print
            value<str>:CALL __INSPECT_SCOPE__ END
            title<str>:"Scope inspecture result"
        END

        CALL print
            value<str>:CALL __INSPECT_VARS__ END
            title<str>:"variables inspecture result"
        END

        CALL print
            value<str>:CALL __INSPECT_FUNCS__ END
            title<str>:"Scope functions result"
        END

        RETURN (GET num1 * GET num2)
    END

    CALL print
        value<str>:CALL __INSPECT_SCOPE__ END
        title<str>:"Scope inspecture result"
    END

    CALL print
        value<str>:CALL __INSPECT_VARS__ END
        title<str>:"variables inspecture result"
    END

    CALL print
        au<str>:"\n\n================================================================================================================ \n"
        value<str>:CALL __INSPECT_FUNCS__ END
        title<str>:"Scope functions result"
    END
    
    CALL multiple num1<float>:5 num2<float>:10 END
    )CODE";
    
    {
        auto r = Runtime::RunTime(code);
        r.add_external_function(print,"print","void",{},true);
        r.add_external_function(Pi,"Pi","float",{});
        Lib::DevFunctions::__init__(r);
    
        auto execute = r.execute_code();
        if (!execute.success){
            std::cout << Errors::formatter(execute.error_info,code) << "\n\n" << std::flush;
            return 1;
        }
    }
    std::cout << std::endl << "======== exec end =========" << std::endl << std::flush;

    return 0;
}