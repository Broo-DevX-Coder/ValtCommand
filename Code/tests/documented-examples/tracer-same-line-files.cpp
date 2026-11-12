#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "valtcommand/runtime.hpp"

int main(int argc, char** argv) {
    if (argc != 2) return 2;

    const std::filesystem::path directory = argv[1];
    std::filesystem::create_directories(directory);
    const auto first = directory / "first.vlt";
    const auto second = directory / "second.vlt";
    std::ofstream(first) << "SET first<int> = 1\n";
    std::ofstream(second) << "SET second<int> = 2\n";

    vltc::Runtime::RunTime runtime(true);
    std::ostringstream event_log;
    runtime.set_debugger_ostream(event_log);
    int stops = 0;
    runtime.set_stop_slot([&](vltc::DevTools::Debugger& debugger,
                              vltc::Scopes::Scope*) {
        ++stops;
        debugger.continue_exec();
    });
    runtime.add_break_points({1}, second.string());

    const auto first_result = runtime.run_file(first.string());
    const auto second_result = runtime.run_file(second.string());
    if (!first_result || !second_result) return 3;

    std::cout << "stops=" << stops << '\n';
    return stops == 1 ? 0 : 1;
}
