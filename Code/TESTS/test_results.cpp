#include "test_helpers.hpp"
#include <algorithm>
#include <tuple>

namespace tests {
void test_results() {
    Suite& results = suite("results");
    vltc::Runtime::RunTime runtime;

    // Every script stores its answer; tests read the typed value through Runtime.
    run_ok(results, runtime, "integer arithmetic", "SET r<int> = 2 + 3 * 4",
           "r", "int", "14");
    run_ok(results, runtime, "string value", "SET r<str> = \"ok\"",
           "r", "str", "ok");
    run_ok(results, runtime, "boolean expression",
           "SET r<bool> = True || False && False", "r", "bool", "true");
    run_ok(results, runtime, "float division", "SET r<float> = 7 / 2",
           "r", "float", "3.5");
    run_ok(results, runtime, "if result",
           "SET r<int> = 0 IF True THEN SET r = 9 END", "r", "int", "9");
    run_ok(results, runtime, "for loop",
           "SET total<int> = 0 FOR SET i<int> = 0; GET i < 4; SET i = GET i + 1 THEN SET total = GET total + GET i END SET r<int> = GET total",
           "r", "int", "6");
    run_ok(results, runtime, "while loop",
           "SET i<int> = 0 SET total<int> = 0 WHILE GET i < 3 THEN SET total = GET total + 1 SET i = GET i + 1 END SET r<int> = GET total",
           "r", "int", "3");

    // Exercise every operator spelling and representative precedence behavior.
    const std::vector<std::tuple<std::string, std::string, std::string, std::string>> operations = {
        {"add", "2 + 3", "float", "5"}, {"subtract", "7 - 3 - 1", "float", "3"},
        {"multiply", "3 * 4", "float", "12"}, {"divide", "7 / 2", "float", "3.5"},
        {"integer divide", "-7 %% 2", "float", "-3"}, {"remainder", "-7 % 2", "float", "-1"},
        {"power right associative", "2 ** 3 ** 2", "float", "512"},
        {"unary plus", "+2", "int", "2"}, {"unary minus", "-2", "int", "-2"},
        {"power binds before unary", "-2 ** 2", "float", "-4"},
        {"equal", "1 == 1", "bool", "true"}, {"not equal", "1 != 2", "bool", "true"},
        {"less", "1 < 2", "bool", "true"}, {"greater", "2 > 1", "bool", "true"},
        {"less equal", "1 <= 1", "bool", "true"}, {"greater equal", "2 >= 2", "bool", "true"},
        {"logical and", "True && False", "bool", "false"},
        {"logical or", "True || False", "bool", "true"},
        {"logical not", "!False", "bool", "true"},
        {"parentheses", "(2 + 3) * 4", "float", "20"},
        {"mixed numeric", "1 + 2.5", "float", "3.5"},
    };
    for (const auto& [name, expression, type, expected] : operations) {
        run_ok(results, runtime, "operator " + name,
               "SET r<" + type + "> = " + expression, "r", type, expected);
    }

    // CRLF must advance the lexer line exactly once, just like LF.
    runtime.reset();
    const auto lf = runtime.run("SET r<int> = 1\nSET r = GET r + 1");
    const auto lf_value = runtime.get_variable("r");
    runtime.reset();
    const auto crlf = runtime.run("SET r<int> = 1\r\nSET r = GET r + 1");
    const auto crlf_value = runtime.get_variable("r");
    check(results, "LF and CRLF", lf && crlf && lf_value && crlf_value &&
          crlf_value.type == "int" && crlf_value.value == "2" &&
          lf_value.value == crlf_value.value,
          "same int:2", crlf_value ? describe(crlf_value) : "failed");

    // Use explicit UTF-8 bytes so MSVC source encoding cannot alter the test.
    runtime.reset();
    const auto utf8 = runtime.run("SET r<str> = \"caf\xC3\xA9\"");
    const auto text = runtime.get_variable("r");
    check(results, "UTF-8 string bytes", utf8 && text && text.value == "caf\xC3\xA9",
          "str:caf\\xC3\\xA9", text ? describe(text) : "failed");
}

void test_documented_examples() {
    Suite& results = suite("documented examples");
    const auto examples_dir = std::filesystem::path("docs") / "V1.0.0" /
                              "Language-references" / "examples";
    const auto expected_dir = std::filesystem::path("docs") / "V1.0.0" /
                              "Language-references" / "exceptions";
    std::vector<std::filesystem::path> examples;
    for (const auto& item : std::filesystem::directory_iterator(examples_dir)) {
        if (item.path().extension() == ".vlt") examples.push_back(item.path());
    }
    std::sort(examples.begin(), examples.end());

    // Keep documented print/input prompts from becoming test output. Assertions
    // below use only RunResult and never inspect captured text.
    struct NullBuffer : std::streambuf { int overflow(int c) override { return c; } } null_buffer;
    auto* old_stdout = std::cout.rdbuf(&null_buffer);

    for (const auto& source : examples) {
        const auto fixture = expected_dir / (source.stem().string() + ".expected");
        std::ifstream manifest(fixture);
        std::string expected_status;
        std::getline(manifest, expected_status);
        if (!manifest || (expected_status != "OK" && expected_status != "ERROR")) {
            check(results, source.filename().string(), false,
                  "OK or ERROR manifest", "missing or invalid manifest");
            continue;
        }

        // Read only the diagnostic category from the captured fixture. The
        // test deliberately ignores formatter layout, carets, paths, and text.
        std::string expected_error_type;
        if (expected_status == "ERROR") {
            std::string line;
            while (std::getline(manifest, line)) {
                const auto colon = line.find(": ");
                if (colon == std::string::npos) continue;
                const auto candidate = line.substr(0, colon);
                if (candidate.ends_with("Error")) {
                    expected_error_type = candidate;
                    break;
                }
            }
            if (expected_error_type.empty()) {
                check(results, source.filename().string(), false,
                      "ERROR manifest with an error category", "category not found");
                continue;
            }
        }

        // These examples depend on long double range or precision. The
        // out-of-range integer case rounds differently when long double has
        // only double precision, so its expected OverflowError is not portable.
#if defined(_WIN32) || defined(__APPLE__)
        if (source.stem() == "types-float-limits" ||
            source.stem() == "types-float-precision" ||
            source.stem() == "types-int-limits" ||
            (source.stem() == "types-int-out-of-range" &&
             std::numeric_limits<long double>::digits <=
                 std::numeric_limits<double>::digits)) {
            ++results.skipped;
            continue;
        }
#endif

        vltc::Runtime::RunTime runtime;
        const auto run = runtime.run_file(source.string(), vltc::Runtime::ScopeMode::Fresh);
        const bool should_succeed = expected_status == "OK";
        const bool matches = should_succeed
            ? static_cast<bool>(run)
            : (!run && run.error.type == expected_error_type);
        const std::string actual = run ? "OK" : "ERROR " + run.error.type;
        const std::string expected = should_succeed ? "OK" : "ERROR " + expected_error_type;
        check(results, source.filename().string(), matches, expected, actual);
    }

    std::cout.rdbuf(old_stdout);
}

void test_runtime_api() {
    Suite& results = suite("runtime API");
    vltc::Runtime::RunTime runtime;

    const auto set = runtime.set_variable("host_value", "7", "int");
    const auto initial = runtime.get_variable("host_value");
    check(results, "set/get variable", set && initial && initial.type == "int" &&
          initial.value == "7", "int:7", initial ? describe(initial) : "failed");
    check(results, "has variable", runtime.has_variable("host_value"), "true", "false");

    const auto persistent = runtime.run("SET host_value = GET host_value + 1");
    const auto updated = runtime.get_variable("host_value");
    check(results, "persistent run scope", persistent && updated && updated.value == "8",
          "int:8", updated ? describe(updated) : "failed");

    const auto fresh = runtime.run("SET temp<int> = 99", vltc::Runtime::ScopeMode::Fresh);
    const auto preserved = runtime.get_variable("host_value");
    check(results, "fresh scope isolation", fresh && preserved && preserved.value == "8",
          "int:8", preserved ? describe(preserved) : "failed");

    const auto bad_type = runtime.set_variable("bad", "x", "unknown");
    check(results, "host rejects unknown type", !bad_type &&
          bad_type.error.stage == vltc::Runtime::ErrorStage::Host,
          "Host error", bad_type ? "success" : bad_type.error.message);
    const auto missing = runtime.get_variable("absent");
    check(results, "host missing variable", !missing &&
          missing.error.stage == vltc::Runtime::ErrorStage::Host,
          "Host error", missing ? "success" : missing.error.message);

    runtime.add_external_function([](vltc::ExternalFunInType) {
        vltc::ReturnResult<vltc::Value> result;
        result.success = true;
        result.value = int64_t{42};
        return result;
    }, "host_native", "int", {});
    const auto script_state = runtime.run(
        "FUNCTION user_fn()->int RETURN 5 END SET user_value<int> = 7");
    runtime.reset();
    const auto removed_function = runtime.run("CALL user_fn END");
    const auto native_after_reset = runtime.run(
        "SET native_result<int> = CALL host_native END");
    const auto native_value = runtime.get_variable("native_result");
    check(results, "reset clears script variables and functions, keeps native functions",
          script_state && !runtime.has_variable("user_value") && !removed_function &&
              native_after_reset && native_value && native_value.value == "42",
          "user entries removed and host_native returns 42",
          "variable=" + std::to_string(runtime.has_variable("user_value")) +
              ", user function error=" + std::to_string(!removed_function) +
              ", native=" + (native_value ? native_value.value : native_value.error.message));

    // Debugger callbacks expose typed events and breakpoints without formatting them.
    vltc::Runtime::RunTime debug_runtime(true);
    std::vector<vltc::DevTools::EventsT> event_types;
    std::vector<std::string> event_strings;
    bool stopped = false;
    auto* debugger = debug_runtime.GetDebugger();
    debugger->set_emit_event_slot([&](vltc::DevTools::event event, std::ostream&) {
        event_types.push_back(event.type);
        event_strings.push_back(event.get_str());
    });
    debugger->set_stop_slot([&](vltc::DevTools::Debugger&, vltc::Scopes::Scope*) {
        stopped = true;
    });
    debugger->add_break_points({1});
    const auto debug_run = debug_runtime.run("SET traced<int> = 1");
    const bool has_runtime_event = std::find(event_types.begin(), event_types.end(),
        vltc::DevTools::EventsT::Runtime) != event_types.end();
    const bool has_analysis_event = std::find(event_types.begin(), event_types.end(),
        vltc::DevTools::EventsT::Analysis) != event_types.end();
    check(results, "debug event callback", debug_run && has_runtime_event && has_analysis_event,
          "Analysis and Runtime event kinds", "missing expected event kind");
    check(results, "runtime breakpoint callback", debug_run && stopped,
          "stop callback invoked at line 1", stopped ? "invoked" : "not invoked");

    bool events_in_order = event_strings.size() > 1;
    for (const auto& expected : event_strings)
        events_in_order = events_in_order && debugger->get_next_event_str() == expected;
    const auto event_end = debugger->get_next_event_str();
    check(results, "next debugger events preserve emission order",
          events_in_order && event_end == "EVENTS END",
          "all events in callback order followed by EVENTS END",
          events_in_order ? event_end : "event order mismatch");

    vltc::Runtime::RunTime native_mutation_runtime;
    native_mutation_runtime.set_variable("x", "1", "int");
    native_mutation_runtime.add_external_function([](vltc::ExternalFunInType input) {
        vltc::Token token{vltc::TokenType::IDENTIFIER, "x", 1, 1, 1, {}};
        auto found = input.ParentScope->search_var(token);
        input.ParentScope->add_var("native_added", "int", int64_t{7});
        if (found.success) {
            found.value->type = "str";
            found.value->value = std::string("changed");
        }
        vltc::ReturnResult<vltc::Value> result;
        result.success = found.success;
        result.value = std::monostate{};
        return result;
    }, "mutate_scope", "void", {});
    const auto native_mutation = native_mutation_runtime.run("CALL mutate_scope END");
    const auto native_x = native_mutation_runtime.get_variable("x");
    const auto native_added = native_mutation_runtime.get_variable("native_added");
    const auto analysis_after_native_mutation = native_mutation_runtime.run(
        "SET from_added<int> = GET native_added");
    check(results, "native callback mutates its live scope",
          native_mutation && native_x && native_x.type == "str" && native_x.value == "changed" &&
              native_added && native_added.value == "7" && !analysis_after_native_mutation &&
              analysis_after_native_mutation.error.stage == vltc::Runtime::ErrorStage::Semantic,
          "runtime values changed; new entry visible at runtime but absent from analysis scope",
          native_x.type + ":" + native_x.value + ", added=" +
              (native_added ? native_added.value : native_added.error.message) +
              ", next analysis stage=" + std::to_string(static_cast<int>(analysis_after_native_mutation.error.stage)));

    const std::string gap_source =
        "SET a<int> = 1\n\nFUNCTION f()->int\nRETURN 2\nEND\nSET b<int> = 3";
    vltc::Runtime::RunTime gap_runtime(true);
    auto* gap_debugger = gap_runtime.GetDebugger();
    gap_debugger->set_emit_event_slot([](vltc::DevTools::event, std::ostream&) {});
    int gap_stops = 0;
    gap_debugger->set_stop_slot([&](vltc::DevTools::Debugger&, vltc::Scopes::Scope*) {
        ++gap_stops;
    });
    gap_debugger->add_break_points({2, 5});
    const auto gap_run = gap_runtime.run(gap_source);
    check(results, "breakpoints on blank and END lines do not stop",
          gap_run && gap_stops == 0,
          "no stop for blank line 2 or END line 5", std::to_string(gap_stops) + " stops");

    vltc::Runtime::RunTime next_gap_runtime(true);
    auto* next_gap_debugger = next_gap_runtime.GetDebugger();
    next_gap_debugger->set_emit_event_slot([](vltc::DevTools::event, std::ostream&) {});
    int next_gap_stops = 0;
    next_gap_debugger->set_stop_slot([&](vltc::DevTools::Debugger& debugger, vltc::Scopes::Scope*) {
        ++next_gap_stops;
        debugger.next_line();
    });
    next_gap_debugger->add_break_points({1});
    const auto next_gap_run = next_gap_runtime.run(gap_source);
    check(results, "next_line skips an empty source line without a runtime event",
          next_gap_run && next_gap_stops == 1,
          "one stop at line 1; temporary line 2 stop never fires", std::to_string(next_gap_stops) + " stops");

    vltc::Runtime::RunTime next_adjacent_runtime(true);
    auto* next_adjacent_debugger = next_adjacent_runtime.GetDebugger();
    next_adjacent_debugger->set_emit_event_slot([](vltc::DevTools::event, std::ostream&) {});
    int adjacent_stops = 0;
    next_adjacent_debugger->set_stop_slot([&](vltc::DevTools::Debugger& debugger, vltc::Scopes::Scope*) {
        ++adjacent_stops;
        if (adjacent_stops == 1) debugger.next_line();
    });
    next_adjacent_debugger->add_break_points({1});
    const auto adjacent_run = next_adjacent_runtime.run("SET a<int> = 1\nSET b<int> = 2");
    check(results, "next_line stops at the adjacent executable line",
          adjacent_run && adjacent_stops == 2,
          "stops at line 1 and line 2", std::to_string(adjacent_stops) + " stops");

    vltc::Runtime::RunTime debugger_mutation_runtime(true);
    debugger_mutation_runtime.set_variable("x", "1", "int");
    std::ostringstream debugger_log;
    debugger_mutation_runtime.set_debugger_ostream(debugger_log);
    auto* mutation_debugger = debugger_mutation_runtime.GetDebugger();
    mutation_debugger->set_emit_event_slot([](vltc::DevTools::event, std::ostream&) {});
    int mutation_stops = 0;
    mutation_debugger->set_stop_slot([&](vltc::DevTools::Debugger&, vltc::Scopes::Scope* scope) {
        ++mutation_stops;
        vltc::Token token{vltc::TokenType::IDENTIFIER, "x", 2, 1, 1, {}};
        auto found = scope->search_var(token);
        if (found.success) {
            found.value->type = "str";
            found.value->value = std::string("debugged");
            scope->add_var("debug_added", "int", int64_t{9});
        }
    });
    mutation_debugger->add_break_points({2});
    const auto debugger_mutation = debugger_mutation_runtime.run("SET x = 1\nSET y<int> = 2");
    const auto debugger_x = debugger_mutation_runtime.get_variable("x");
    const auto debugger_added = debugger_mutation_runtime.get_variable("debug_added");
    const auto analysis_after_debugger_mutation = debugger_mutation_runtime.run(
        "SET from_debugger<str> = GET x");
    check(results, "debugger callback mutates its live scope",
          debugger_mutation && mutation_stops == 1 && debugger_x && debugger_x.type == "str" &&
              debugger_x.value == "debugged" && debugger_added && debugger_added.value == "9" &&
              !analysis_after_debugger_mutation &&
              analysis_after_debugger_mutation.error.stage == vltc::Runtime::ErrorStage::Semantic,
          "callback changes runtime scope; analysis still sees original int entry",
          "stops=" + std::to_string(mutation_stops) + ", x=" + debugger_x.type + ":" +
              debugger_x.value + ", added=" +
              (debugger_added ? debugger_added.value : debugger_added.error.message) +
              ", next analysis stage=" + std::to_string(static_cast<int>(analysis_after_debugger_mutation.error.stage)));
}
}
