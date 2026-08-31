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
#include "Parser.hpp"

// ==================================================================
// Entry point function
// ==================================================================
int main () {
    std::string code = R"CODE(
    CALL print 
        value<str>:"Hellow n****"
    END
    CALL sum
        _1st<float>:1.5
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

    std::cout << "==== Start Parser ====" << std::endl << std::flush;

    Parser p(all_t);
    auto t = p.get_program_node();

    std::cout << t->get_str(0) << std::endl << std::flush;

    std::cout << "==== End Parser ====" << std::endl << std::flush;
    

    return 0;
}