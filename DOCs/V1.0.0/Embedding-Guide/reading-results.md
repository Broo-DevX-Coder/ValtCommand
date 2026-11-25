# Reading results

There are three distinct result/output paths:

1. **Top-level return:** `RunResult.value` and `.type` describe an execution result when present; for ordinary top-level assignments, use `get_variable()` to read the named global as text and type.
2. **A dedicated stream:** the debugger stream can be redirected with `set_debugger_ostream(std::ostream&)` when debug mode is enabled. This captures debugger event output, not ordinary language `print` output. The stock STDIO implementation writes to `std::cout`.
3. **Scope inspection:** the debugger stop callback receives the active `Scopes::Scope*`; its visible variable/function tables return pointers to entries. `get_variable()` reads a global by name as text and type. Use inspection for live state, with care around pointer lifetime and mutation. `DevTools::InspecturesReturn::JSON` is serialized JSON text (`std::string`), so consumers do not need a public nlohmann header.

Example: [results.cpp](examples/results.cpp).
