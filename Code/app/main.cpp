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
#include "Lexar.hpp"

// ==================================================================
// Entry point function
// ==================================================================
int main () {
    std::string code = R"CODE(
    CALL print 
        value<str>:"Hellow n****"
    END
    CALL sum
        _1st<int>:1.5
        _2nd<int>: 5
    END
    CALL boolean_return
        value1<bool>:True
        value2<bool>:False
    END
    )CODE";

    std::cout << "==== Start Lexar ====" << std::endl << std::flush;

    Lexar l(code);
    auto all_t = l.get_all_tokens();
    for (auto& tk: all_t) {
        tk.print();
    }

    std::cout << "==== End Lexar ====" << std::endl << std::flush;

    return 0;
}