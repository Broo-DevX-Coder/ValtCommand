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
// ============================================================
// ValtCommand - Complete Test Suite
// ============================================================


// ============================================================
// 1. Arithmetic Operations
// ============================================================

CALL print value<int>:10 + 5 END
CALL print value<int>:10 - 5 END
CALL print value<int>:10 * 5 END
CALL print value<int>:20 / 5 END
CALL print value<int>:24 %% 5 END
CALL print value<int>:10 % 3 END
CALL print value<int>:2 ** 3 END

// Expected: 15, 5, 50, 4, 4, 1, 8


// ============================================================
// 2. Comparison Operators
// ============================================================

CALL print value<bool>:10 > 5 END
CALL print value<bool>:10 < 5 END
CALL print value<bool>:10 >= 10 END
CALL print value<bool>:5 <= 10 END
CALL print value<bool>:10 == 10 END
CALL print value<bool>:10 != 5 END

// Expected: True, False, True, True, True, True


// ============================================================
// 3. Logical Operators
// ============================================================

CALL print value<bool>:True && True END
CALL print value<bool>:True && False END
CALL print value<bool>:True || False END
CALL print value<bool>:False || False END
CALL print value<bool>:!True END
CALL print value<bool>:!False END

// Expected: True, False, True, False, False, True


// ============================================================
// 4. Operator Precedence
// ============================================================

CALL print value<int>:2 + 3 * 4 END
CALL print value<int>:2 ** 3 ** 2 END
CALL print value<int>:(-2) ** 2 END
CALL print value<int>:-2 ** 2 END
CALL print value<int>:2 + 3 * 2 ** 3 %% 5 END

// Expected: 14, 512, 4, -4, 6


// ============================================================
// 5. Variables
// ============================================================

SET a<int> = 10
SET b<int> = 20

CALL print value<int>:GET a END
CALL print value<int>:GET b END

SET a = GET a + GET b

CALL print value<int>:GET a END

// Expected: 10, 20, 30


// ============================================================
// 6. Conditional Statements
// ============================================================

SET score<int> = 85

IF (GET score) >= 50 THEN
    CALL print value<str>:"Passed" END
ELSE THEN
    CALL print value<str>:"Failed" END
END

IF (GET score) < 50 THEN
    CALL print value<str>:"Failed" END
ELSE THEN
    CALL print value<str>:"Passed" END
END

// Expected: Passed, Passed


// ============================================================
// 7. While Loop
// ============================================================

SET counter<int> = 0

WHILE (GET counter) < 5 THEN
    CALL print value<int>:GET counter END
    SET counter = GET counter + 1
END

// Expected: 0, 1, 2, 3, 4


// ============================================================
// 8. Functions
// ============================================================

FUNCTION add (
    a<int>
    b<int>
)->int
    RETURN (GET a) + (GET b)
END

FUNCTION multiply (
    a<int>
    b<int>
)->int
    RETURN (GET a) * (GET b)
END

CALL print value<int>:(CALL add a<int>:10 b<int>:20 END) END
CALL print value<int>:(CALL multiply a<int>:5 b<int>:6 END) END

// Expected: 30, 30


// ============================================================
// 9. Nested Function Calls
// ============================================================

CALL print value<int>:CALL add a<int>:10 b<int>:20 END END

CALL print value<int>:CALL multiply a<int>:2 b<int>:CALL add a<int>:3 b<int>:2 END END END

// Expected: 30, 10


// ============================================================
// 10. Complex Expressions
// ============================================================

CALL print value<int>:(((((12 + 8) * 3 ** 2) - (144 %% 4)) + ((7 ** 3) % 10) - (-5 * 6)) * 2 + (100 %% (2 ** 3))) END

// Expected: 366


// ============================================================
// 11. Error Handling Tests
// ============================================================

// Uncomment these one at a time to test errors.
// Some errors may stop execution, depending on your runtime.

// CALL print value<int>:GET undefined_variable END
// CALL print value<int>:"wrong type" END
// SET invalid<int> = "text"


// ============================================================
// End of Test Suite
// ============================================================

CALL print value<str>:"All test sections completed." END
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