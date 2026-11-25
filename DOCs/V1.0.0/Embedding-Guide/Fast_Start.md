# ValtCommand 1.0.0 — C++ Embedding Guide

**ValtCommand** is an embeddable, C++20 interpreted scripting language designed for lightweight application scripting and configuration logic (similar in integration philosophy to Lua).

This guide covers building, linking, executing scripts, exchanging data, registering native C++ functions, debugging, and API references for **ValtCommand v1.0.0**.

---

## 1. Requirements & Installation

- **Compiler:** C++20 compatible compiler (`g++ 11+`, `Clang 14+`, `MSVC 2022+`).
- **Build System:** CMake `3.20+`.
- **Dependencies:** Zero public third-party dependencies. Internal dependencies (`fmt` and `nlohmann_json`) are encapsulated privately within the compiled library.

### Building and Installing from Source

```sh
cmake -S Code -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
cmake --build build --config Release
cmake --install build --config Release
```

### Integrating via CMake

Once installed, consume the exported `ValtCommand::valtcommand` target in your host application's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(HostApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ValtCommand CONFIG REQUIRED)

add_executable(host_app main.cpp)
target_link_libraries(host_app PRIVATE ValtCommand::valtcommand)
```

Include the single umbrella header in your C++ source files:

```cpp
#include <valtcommand/valtcommand.hpp>
```

> **Runtime Library Discovery:** On Linux and macOS, ValtCommand configures `RPATH` (`$ORIGIN;$ORIGIN/../lib` and `@loader_path;@loader_path/../lib`). On Windows, ensure `valtcommand.dll` is in the executable directory or on `PATH`.

---

## 2. Quick Start

```cpp
#include <iostream>
#include <valtcommand/valtcommand.hpp>

int main() {
    vltc::Runtime::RunTime runtime;

    auto result = runtime.run(R"(
        SET greeting<str> = "Hello from ValtCommand!"
        SET answer<int> = (10 + 5) * 2
    )");

    if (!result) {
        std::cerr << "Error [" << result.error.type << "] at line "
                  << result.error.line << ": " << result.error.message << "\n";
        return 1;
    }

    auto var = runtime.get_variable("answer");
    if (var) {
        std::cout << "answer (" << var.type << ") = " << var.value << "\n";
    }

    return 0;
}
```

---

## 3. Running Code & Managing State

### Execution Modes (`ScopeMode`)

`RunTime` maintains three internal global scopes (runtime, semantic analysis, and run-analysis). You control how state persists across calls using `vltc::Runtime::ScopeMode`:

- **`ScopeMode::Persistent` (Default):** Variables and user-defined functions remain alive across subsequent `run()` and `run_file()` calls.
- **`ScopeMode::Fresh`:** Executes the script in isolated temporary scopes without polluting or reading persistent script globals.

```cpp
// Executes from a string
auto r1 = runtime.run("SET x<int> = 42", vltc::Runtime::ScopeMode::Persistent);

// Executes from a file path (returns ErrorStage::IO if file cannot be opened)
auto r2 = runtime.run_file("scripts/init.vlt");
```

### Error Handling (`RunResult`)

`run()`, `run_file()`, `set_variable()`, and `get_variable()` do not throw exceptions for script errors; they return a `vltc::Runtime::RunResult`:

- `bool(result)` / `result.ok`: `true` on success, `false` on failure.
- `result.value` & `result.type`: Formatted string value and language type name (when applicable).
- `result.error`: Contains `stage` (`IO`, `Lexer`, `Parser`, `Analysis`, `Runtime`, `Host`), `type`, `message`, `line` (zero-based), `column`, and `file`.

### Resetting & Host Variable Access

- **`runtime.reset()`:** Clears all script-created global variables and user-defined functions while preserving all host-registered native functions and native constants.
- **`runtime.set_variable(name, value_str, type_str, is_const = false)`:** Injects or updates a global variable (`int`, `float`, `bool`, or `str`). Rejects overwriting registered native constants.
- **`runtime.get_variable(name)`:** Retrieves a global variable's string representation and type.
- **`runtime.has_variable(name)`:** Returns `true` if the global variable exists.

---

## 4. Registering Native C++ Functions & Constants

Host applications can expose C++ callbacks and immutable constants to ValtCommand scripts. Both survive `runtime.reset()` and `ScopeMode::Fresh` executions.

### Supported Value Types (`vltc::Value`)

| ValtCommand Type | C++ `std::variant` Alternative |
|---|---|
| `void` | `std::monostate` |
| `int` | `int64_t` |
| `float` | `long double` |
| `str` | `std::string` |
| `bool` | `bool` |

### Registering a Native Function

Callbacks must match `ExternalFuncType`: `ReturnResult<Value>(ExternalFunInType)`.

```cpp
#include <valtcommand/valtcommand.hpp>

vltc::ReturnResult<vltc::Value> native_add(vltc::ExternalFunInType input) {
    vltc::ReturnResult<vltc::Value> res;

    auto a = std::get<int64_t>(input.arguments.at("a"));
    auto b = std::get<int64_t>(input.arguments.at("b"));

    res.success = true;
    res.value = a + b;
    return res;
}

void register_bindings(vltc::Runtime::RunTime& runtime) {
    // Define parameters: { name, { type, is_const, is_required, is_any, default_value } }
    std::unordered_map<std::string, vltc::Runtime::ExternalFunctionArgument> args = {
        {"a", {"int", false, true, false, int64_t{0}}},
        {"b", {"int", false, true, false, int64_t{0}}}
    };

    runtime.add_external_function(native_add, "add_numbers", "int", args);
    runtime.add_external_constant("MAX_ITEMS", int64_t{100});
}
```

In ValtCommand script:
```vlt
SET total<int> = CALL add_numbers a<int>:10 b<int>:GET MAX_ITEMS END
```

> **Library Pattern:** Built-in and modular native libraries follow the convention `vltc::Lib::<LibraryName>::__init__(vltc::Runtime::RunTime&)`.

---

## 5. Debugging, Tracing & Events

Pass `true` to the `RunTime` constructor to enable the embedded debugger:

```cpp
vltc::Runtime::RunTime runtime(true);
vltc::DevTools::Debugger* dbg = runtime.GetDebugger();
```

### Event Categories (`EventsT`)

Every lifecycle and execution step emits structured `event` objects categorized as:
- `EventsT::Info`
- `EventsT::Analysis`
- `EventsT::Runtime` (emitted once per executed AST node; drives line tracing and breakpoints)
- `EventsT::Debug`
- `EventsT::Warning`
- `EventsT::Error`

### Custom Event & Breakpoint Handlers

By default, hitting a breakpoint pauses execution and reads interactive console commands (`c`/`continue`, `n`/`nextl`, `e`/`exit`, `sv`, `sf`, `sc`) from `std::cin`. **Embedded hosts should replace the stop slot** to integrate with their own UI or event loop:

```cpp
// Redirect debugger log output
std::ostringstream dbg_log;
runtime.set_debugger_ostream(dbg_log);

// Custom event listener
dbg->set_emit_event_slot([](vltc::DevTools::event e, std::ostream& out) {
    out << e.get_str();
});

// Custom breakpoint handler (synchronous: returning resumes execution)
dbg->set_stop_slot([](vltc::DevTools::Debugger& d, vltc::Scopes::Scope* active_scope) {
    auto vars = active_scope->get_variables_table();
    // Inspect variables or call d.next_line() to step to the next line
});

// Set breakpoints on lines 2 and 5 (empty path matches any file)
dbg->add_break_points({2, 5}, "script.vlt");
```

### Inspecting Scopes

Inside a stop callback or native function, you can inspect live variables and functions via `Scope::get_variables_table()` and `Scope::get_functions_table()`, or use the inspector utilities (`ScopeInspect`, `VariablesInspect`, `FunctionsInspect`), which serialize their `.JSON` output directly as a `std::string`.

---

## 6. Known Limitations (v1.0.0)

1. **Default Debugger `exit` Command:** Invoking `Debugger::exit()` (or typing `e` in the default console stop handler) calls `std::exit(0)`, terminating the host process. Always override `set_stop_slot()` in embedded applications.
2. **No Single-Symbol Deletion:** `Scope` does not expose an erase method for individual variables or functions. Use `RunTime::reset()` to clear all script-defined symbols at once.
3. **Mutating Live Scopes in Callbacks:** Modifying `ParentScope` directly from a native callback updates runtime state but does not update parallel static analysis scopes, which can cause subsequent analysis passes to disagree with runtime state.
4. **Standard Output Stream:** `set_debugger_ostream()` redirects debugger output only. Built-in script `print` / `println` statements write directly to `std::cout`.
5. **Re-entrancy:** Do not call `run()`, `run_file()`, `reset()`, or `Initialyze()` on the same `RunTime` instance from inside an active native or debugger callback.

---

## 7. Quick API Reference

### `vltc::Runtime::RunTime`
- `RunTime(bool DebugMode = false)` / `explicit RunTime(const std::string& code, bool DebugMode = false)`
- `void Initialyze(bool DebugMode = false)`
- `DevTools::Debugger* GetDebugger()`
- `void set_debugger_ostream(std::ostream&)` / `const std::ostream* get_debugger_ostream() const`
- `RunResult run(const std::string& code, ScopeMode mode = ScopeMode::Persistent)`
- `RunResult run_file(const std::string& path, ScopeMode mode = ScopeMode::Persistent)`
- `void reset()`
- `RunResult set_variable(const std::string& name, const std::string& value, const std::string& type, bool is_constant = false)`
- `RunResult get_variable(const std::string& name)`
- `bool has_variable(const std::string& name)`
- `void add_external_function(ExternalFuncType, const std::string& name, const std::string& return_type, std::unordered_map<std::string, ExternalFunctionArgument>, bool is_any = false)`
- `bool add_external_constant(const std::string& name, const Value& value)`

### `vltc::Scopes::Scope`
- `size_t get_id()` / `Scope* get_parent()` / `size_t get_depth()`
- `vars_table get_variables_table()` / `funcs_table get_functions_table()`
- `RVar* add_var(...)` / `Function* add_function(...)`
- `RVar* search_var(Token&)` / `Function* search_function(Token&)`

### `vltc::DevTools::Debugger`
- `void set_ostream(std::ostream&)` / `const std::ostream* get_ostream() const`
- `void emit(event)` / `void set_emit_event_slot(EmitSlotType)`
- `std::string get_next_event_str()` / `std::string get_all_events_strs()`
- `void add_break_points(const std::vector<size_t>& lines, const fs::path& file = "")`
- `void check_break_point(size_t line, Scopes::Scope* scope, const fs::path& file = "")`
- `void set_stop_slot(StopSlotType)`
- `void next_line()` / `void continue_exec()` / `void exit()`