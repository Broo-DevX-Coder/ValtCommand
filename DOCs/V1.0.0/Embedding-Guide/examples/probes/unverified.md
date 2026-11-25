# Follow-up probes for unverified behavior

These are exact checks to run before depending on the named behavior. They are not part of the passing CTest set because results are platform- or implementation-sensitive.

## macOS and Windows package consumption

Run the package build/install commands in `../installation.md` on the target OS, then configure and build this directory with `CMAKE_PREFIX_PATH` pointing to the install prefix. Run `ctest --test-dir <build-dir> --output-on-failure`. On Windows, additionally launch `host_file.exe` with the provided `sample.vlt` while the DLL is placed in the documented package runtime search path. This container run only validated Linux.

## Mutation of a runtime-owned live scope inside a native callback

Compile against the installed package and run this standalone probe. It changes an existing value through `ParentScope` during a callback, then checks whether subsequent execution observes it. This is not a supported runtime guarantee.

```cpp
#include <valtcommand/valtcommand.hpp>
#include <cassert>
int main() {
  vltc::Runtime::RunTime rt;
  rt.set_variable("x", "1", "int");
  rt.add_external_function([](vltc::ExternalFunInType in) {
    vltc::Token t{vltc::TokenType::IDENTIFIER, "x", 1, 1, 1, {}};
    auto found = in.ParentScope->search_var(t);
    vltc::ReturnResult<vltc::Value> out;
    out.success = found.success;
    if (found) found.value->value = int64_t{9};
    out.value = int64_t{0};
    return out;
  }, "mutate", "int", {});
  auto result = rt.run("CALL mutate END");
  assert(result);
  auto observed = rt.get_variable("x");
  // Record observed.value on each supported build; do not assume it is 9.
}
```

## Debugger `next_line` and blank/END line stopping

Use `RunTime(true)`, install a stop slot that increments a counter and calls `next_line()`, then call `run_file` on a fixture with one statement, a blank line, an `END`, and another statement. Record each stop and event line. `debugger.cpp` validates continue and a breakpoint, but not every parser/AST node's line-event behavior.

## Internal lexer and conversion helpers

The lexer tables and conversion/type helper declarations have moved to `Code/include/detail/globals.hpp`, which is not installed with the package. They are not supported consumer API and are therefore excluded from package-level probes. If changing these implementation details, test the interpreter's language-reference examples from a source build.
