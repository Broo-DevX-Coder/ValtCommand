#include "valtcommand/runtime.hpp"
#include "detail/errors.hpp"

using namespace vltc;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
   if (argc < 2) {
       std::cerr << "Usage: valtcommand <file.vc>\n";
       return 1;
   }

    const char* fp = argv[1];

    std::ifstream file(fp);
    if (!file) {
        std::cerr << "Cannot open file: " << fp << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Runtime::RunTime r(false);

    r.set_ostream(std::clog);
    auto result = r.run_file(fp);
    if (!result) {
        std::cout << Errors::formatter(result.error.info,source) << std::endl;
        std::cout << result.error.message << std::endl;
        return 1;
    }
    return 0;
}

/*

// ===========================

#include "detail/lexer.hpp"

int main1() {
    std::string code = R"t(
// Version: 1.0.0
SET result<str> = CALL format
    text<str>:"Name: {name}, Age: {age}, Active: {active}"
    name<str>:"Developer"
    age<int>:18
    active<bool>:true
END
CALL println value<str>:GET result END
    )t";

    auto l = Lexar(code);

    auto all_tokens = l.get_all_tokens();

    for (auto& t: all_tokens) {
        t.print();
    }
    
    return 0;
}
    
*/