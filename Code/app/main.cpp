#include "valtcommand/runtime.hpp"
#include "detail/errors.hpp"

#include <CLI11.hpp>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace vltc;

namespace fs = std::filesystem;

namespace {

/// @brief Result of an operation: success status, a value, and a message when it fails
/// @tparam RType Type of the result value
template<typename RType>
struct ReturnResult {
    bool success;        ///< Success status (true / false)
    RType value;         ///< Result value
    std::string Message; ///< Error message when `success` is false

    explicit operator bool() const { return success; }
};

/// @brief Parse the breakpoints given by the user (--break)
/// @param points Each item is `path:line,line,...` or `line,line,...` (for the main file)
/// @param main_file_path Path of the main file (used when an item has no path)
/// @return ReturnResult with a map: path (string) -> list of lines (size_t)
ReturnResult<std::unordered_map<std::string, std::vector<size_t>>>
get_breakpoints_list(
    const std::vector<std::string>& points,
    const std::string& main_file_path
) {
    std::unordered_map<std::string, std::vector<size_t>> results;

    for (const auto& p : points) {
        std::string file = main_file_path;
        std::string lines_text = p;

        // Split the path from the lines at the LAST ':'
        auto pos = p.rfind(':');
        if (pos != std::string::npos) {
            file = p.substr(0, pos);
            lines_text = p.substr(pos + 1);

            if (file.empty()) {
                return {
                    .success = false,
                    .Message = "Empty file path in breakpoint: " + p
                };
            }
        }

        // Split the lines by ','
        std::stringstream ss(lines_text);
        std::string item;
        bool found_any = false;

        while (std::getline(ss, item, ',')) {
            found_any = true;

            // Only digits are allowed
            const bool digits_only =
                !item.empty() &&
                std::all_of(item.begin(), item.end(),
                    [](unsigned char c) { return std::isdigit(c); });

            if (!digits_only) {
                return {
                    .success = false,
                    .Message = "Invalid breakpoint: " + p
                };
            }

            size_t line = 0;
            try {
                line = std::stoul(item);
            } catch (...) {
                return {
                    .success = false,
                    .Message = "Line number is too big: " + p
                };
            }

            if (line == 0) {
                return {
                    .success = false,
                    .Message = "Lines start from 1: " + p
                };
            }

            // Add each line only once per file
            auto& lines = results[file];
            if (std::find(lines.begin(), lines.end(), line) == lines.end()) {
                lines.push_back(line);
            }
        }

        if (!found_any) {
            return {
                .success = false,
                .Message = "No line numbers in breakpoint: " + p
            };
        }
    }

    return {
        .success = true,
        .value = std::move(results),
        .Message = ""
    };
}

} // namespace


int main(int argc, char* argv[]) {
    // ==================================================================
    // Initialize the CLI application
    // ==================================================================
    CLI::App app{
        "ValtCommand - A lightweight command-based interpreted language.",
        "valtc"
    };

    app.set_version_flag(
        "-v,--version",
        "ValtCommand Interpreter 1.0.0 \n"
        "Copyright (C) 2026 Broo-DevX-Coder \n"
        "GitHub: https://github.com/Broo-DevX-Coder/ValtCommand \n"
        "Licensed under the MIT License. \n"
        "This software is provided \"AS IS\", without warranty \n"
        "of any kind. See the LICENSE file for details."
    );

    // ==================================================================
    // CLI arguments storage
    // ==================================================================

    // Flags
    bool debug_mode = false;
    bool print_logs = false;

    // Option values
    std::string main_file_path;
    std::string eval_code;
    std::string logs_file;
    std::vector<std::string> break_points_strings;

    // ==================================================================
    // Flags
    // ==================================================================

    app.add_flag(
        "-d,--debug,--no-debug{false}",
        debug_mode,
        "Enable or disable the runtime's debug mode.\n"
        "Required for --break, --logs and --log-file to have any effect.\n"
        "Default: disabled."
    );

    app.add_flag(
        "-l,--logs,--no-logs{false}",
        print_logs,
        "Print debugger log messages to the standard error stream.\n"
        "Only works with --debug.\n"
        "Ignored if --log-file names a valid file."
    );

    // ==================================================================
    // Options
    // ==================================================================

    auto* file_option = app.add_option(
        "-f,--file",
        main_file_path,
        "Path of the source file to run. It must exist and be a regular file.\n"
        "Also the default file for --break entries that have only line numbers.\n"
        "Cannot be used with --eval."
    )->type_name("FILE");

    auto* eval_option = app.add_option(
        "-e,--eval",
        eval_code,
        "Source code to run directly, without a file. Must not be empty.\n"
        "Errors from it are shown without a file name.\n"
        "Cannot be used with --file."
    )->type_name("CODE");

    app.add_option(
        "--log-file,--logsf",
        logs_file,
        "Write debugger log messages to FILE (created or overwritten).\n"
        "Only works with --debug.\n"
        "Takes priority over --logs. Ignored if FILE is a directory."
    )->type_name("FILE");

    app.add_option(
        "--break",
        break_points_strings,
        "Register breakpoints. Each value follows one of these forms:\n"
        "  PATH:LINE[,LINE...]   lines in the given file\n"
        "  LINE[,LINE...]        lines in the file given by --file\n"
        "Accepts several values. Lines start at 1.\n"
        "Only works with --debug."
    )->type_name("BREAKPOINT");

    // ==================================================================
    // Option relationships
    // ==================================================================

    // A source file and direct evaluation cannot be used together
    file_option->excludes(eval_option);
    eval_option->excludes(file_option);

    // ==================================================================
    // Help footer
    // ==================================================================

    app.footer(
        "Examples:\n"
        "  valtc --file main.vlt\n"
        "  valtc -f main.vlt --debug\n"
        "  valtc --eval 'CALL print value<str>:\"Hello World!\" END'\n"
        "  valtc --debug --logs --file main.vlt\n"
        "  valtc --debug --break main.vlt:1,2,3\n"
        "  valtc --debug --break main.vlt:1,2,3 utils.vlt:5,10\n"
        "  valtc --debug --break=main.vlt:1,2,3 utils.vlt:5,10\n"
        "  valtc --file main.vlt --logsf runtime.log\n"
        "\n"
        "Documentation:\n"
        "  https://github.com/Broo-DevX-Coder/ValtCommand"
    );

    // ==================================================================
    // Parse arguments
    // ==================================================================

    CLI11_PARSE(app, argc, argv);

    // ==================================================================
    // Initialize the runtime
    // ==================================================================
    Runtime::RunTime r(debug_mode);

    // ==================================================================
    // Debugger settings (only when debug mode is on)
    // ==================================================================
    std::ofstream LogsFileOFStream;    // Output stream of the logs file
    std::ostream NullOstream(nullptr); // Null output stream (discards everything)

    if (debug_mode) {

        // By default, discard the debugger output
        r.set_debugger_ostream(NullOstream);

        if (!logs_file.empty() && !fs::is_directory(logs_file)) {
            // Write the debugger output to the logs file
            LogsFileOFStream = std::ofstream(logs_file);
            r.set_debugger_ostream(LogsFileOFStream);

        } else if (print_logs) {
            // Write the debugger output to the console (stderr)
            r.set_debugger_ostream(std::clog);
        }

        // Parse the breakpoints list
        auto break_points_r = get_breakpoints_list(break_points_strings, main_file_path);
        if (!break_points_r) {
            std::cerr << break_points_r.Message << std::endl;
            return 2;
        }

        // Add all breakpoints to the runtime
        for (auto& [path, points] : break_points_r.value)
            r.add_break_points(points, path);
    }

    // ==================================================================
    // Run the code
    // ==================================================================
    vltc::Runtime::RunResult run_result; // Result of running the code

    if (fs::exists(main_file_path) && fs::is_regular_file(main_file_path)) {

        // A file was given (-f, --file)
        run_result = r.run_file(main_file_path);

    } else if (!eval_code.empty()) {

        // Direct code was given (-e, --eval)
        run_result = r.run(eval_code);

    } else if (!main_file_path.empty()) {
        std::cerr << "Cannot open file: " << fs::absolute(main_file_path).lexically_normal() << std::endl;
        return 3;

    } else {
        std::cerr << "No work to do ..." << std::endl;
        return 1;
    }

    // ==================================================================
    // Error handling
    // ==================================================================
    if (!run_result) {
        std::string error_source_code; // The code where the error happened

        if (!run_result.error.info.file.empty()) {
            // The error belongs to a file: read its source code
            std::ifstream file(run_result.error.info.file);
            std::stringstream buffer;
            buffer << file.rdbuf();
            error_source_code = buffer.str();

        } else {
            // No file path: the error comes from the eval code
            error_source_code = eval_code;
        }

        std::cerr << '\n';
        std::cerr << Errors::formatter(run_result.error.info, error_source_code) << std::endl;

        if (!run_result.error.info.file.empty()) {
            std::cerr << "File: "
                      << fs::absolute(run_result.error.info.file).lexically_normal().generic_string()
                      << std::endl << std::endl;
        }
        return 2;
    }

    return 0;
}