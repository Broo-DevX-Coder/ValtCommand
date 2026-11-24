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

    runtime.reset();
    check(results, "reset clears script state", !runtime.has_variable("host_value"),
          "false", "true");

    // Debugger callbacks expose typed events and breakpoints without formatting them.
    vltc::Runtime::RunTime debug_runtime(true);
    std::vector<vltc::DevTools::EventsT> event_types;
    bool stopped = false;
    debug_runtime.set_emit_event_slot([&](vltc::DevTools::event event, std::ostream&) {
        event_types.push_back(event.type);
    });
    debug_runtime.set_stop_slot([&](vltc::DevTools::Debugger&, vltc::Scopes::Scope*) {
        stopped = true;
    });
    debug_runtime.add_break_points({1});
    const auto debug_run = debug_runtime.run("SET traced<int> = 1");
    const bool has_runtime_event = std::find(event_types.begin(), event_types.end(),
        vltc::DevTools::EventsT::Runtime) != event_types.end();
    const bool has_analysis_event = std::find(event_types.begin(), event_types.end(),
        vltc::DevTools::EventsT::Analysis) != event_types.end();
    check(results, "debug event callback", debug_run && has_runtime_event && has_analysis_event,
          "Analysis and Runtime event kinds", "missing expected event kind");
    check(results, "runtime breakpoint callback", debug_run && stopped,
          "stop callback invoked at line 1", stopped ? "invoked" : "not invoked");
}
}
