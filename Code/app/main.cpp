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
        CALL println
            value<str>: CALL format
                text<str>:"Hello {name}, age: {age}"
                name<str>:"World"
                age<int>:18
            END
        END
    )RT";

    Runtime::RunTime r(true);

    if (r.GetDebugger())
        r.GetDebugger()->add_break_points({{1}});

    auto result = r.run(code,Runtime::ScopeMode::Persistent);
    if (!result) {
        std::cout << Errors::formatter(result.error.info,code) << std::endl;
        return 1;
    }
    return 0;
}