#include "valtcommand/runtime.hpp"
#include "detail/errors.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: valtcommand <file.vc>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Runtime::RunTime r(false);

    if (r.GetDebugger())
        r.GetDebugger()->add_break_points({{7}},"");

    r.set_ostream(std::clog);

    auto result = r.run_file(argv[1]);
    if (!result) {
        std::cout << Errors::formatter(result.error.info,source) << std::endl;
        return 1;
    }
    return 0;

    return 0;
}