#include "API/Runtime.hpp"
#include "Errors.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

int main1() {
    Runtime::RunTime rt;

    std::string path = "/home/broodev/Desktop/PG/ValtCommand/Tests/arithmetic.vlt";

    std::ifstream in(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << in.rdbuf();
    std::string code = buffer.str();

    // ---- 1) Run the whole script ----
    auto r = rt.run_file(path);
    if (!r) {
        std::cerr << Errors::formatter(r.error.info,code);
    }


    return 0;
}

int main() {
    std::string code = R"RT(  
        SET name<str> = CALL input prompt<str>:"Name: " END
        CALL println value<str>:GET name END
    )RT";

    Runtime::RunTime r;
    r.run(code,Runtime::ScopeMode::Persistent);
    return 0;
}