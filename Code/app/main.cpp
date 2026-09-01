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
        value0<str>:"Hellow man 1"
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
    auto mnode = p.get_module_node();
    if (!mnode.success){
        std::cout << "Parser error" << std::endl << std::flush;
        std::cout << mnode.Message << std::endl << std::flush;
        return 1;
    }

    std::cout << mnode.value->get_str() << std::endl << std::flush;

    std::cout << "==== End Parser ====" << std::endl << std::flush;
    std::cout << "==== Start running ====" << std::endl << std::flush;

    auto exec_r = mnode.value->exec();
    if (!exec_r.success){
        std::cout << "Runtime error" << std::endl << std::flush;
        std::cout << exec_r.Message << std::endl << std::flush;
        return 1;
    }

    std::cout << "==== End running ====" << std::endl << std::flush;

    return 0;
}