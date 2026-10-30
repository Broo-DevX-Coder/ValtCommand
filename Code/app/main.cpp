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
        FOR     
            SET i<int> = 0;
            (GET i) <= 10;
            SET i = (GET i) +1
        THEN
            CALL println value<str>:
                CALL format text<str>: "Loop number: {num}" num<int>:(GET i) END
            END
        END
    )RT";

    Runtime::RunTime r(true);

    if (r.GetDebugger())
        r.GetDebugger()->add_break_points({{7}},"");

    r.set_ostream(std::clog);

    auto result = r.run(code,Runtime::ScopeMode::Persistent);
    if (!result) {
        std::cout << Errors::formatter(result.error.info,code) << std::endl;
        return 1;
    }
    return 0;
}