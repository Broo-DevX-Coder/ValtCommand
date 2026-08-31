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
#include "Standards/init.hpp"

// ==================================================================
// Entry point function
// ==================================================================
int main () {

    // ====== initialyze standards ==========
    Standardes::__init__();

    std::string code = R"CODE(
    CALL print 
        value0<str>:"Hellow n****"
    END
    CALL print 
        value<str>:"Hellow n****"
        value2<str>:"Hellow n****"
        value3<str>:"Hellow n****"
        value4<str>:"Hellow n****"
    END
    CALL printf
        value<str>:"Hellow n****"
        value2<str>:"Hellow n****"
        value3<str>:"Hellow n****"
        value4<str>:"Hellow n****"
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
    auto pnode = p.get_program_node();

    std::cout << pnode->get_str(1) << std::endl << std::flush;

    std::cout << "==== End Parser ====" << std::endl << std::flush;
    std::cout << "==== Start running ====" << std::endl << std::flush;

    pnode->exec();

    std::cout << "==== End running ====" << std::endl << std::flush;
    

    return 0;
}