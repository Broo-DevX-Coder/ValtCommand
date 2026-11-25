# Known limitations

- The default debugger's `exit` command calls `std::exit(0)` and kills the entire host process. Install a custom stop slot; do not invoke `Debugger::exit()` in an in-process host.
- `Scope` has no public erase operation. `add_var`/`add_function` replace an entry of the same name. For complete script-state cleanup use `RunTime::reset()`, which removes user variables and functions and reinstalls host native functions and constants.
- `RunTime::set_variable` updates the runtime's global run, analysis, and semantic scopes. It is text-parsed and accepts `int`, `float`, `bool`, and `str`; direct access to runtime-owned scopes is not exposed except through debugger callback pointers.
- A host callback can mutate its received `ParentScope`, but it does not update the runtime's parallel analysis scopes. This can make analysis and execution disagree; avoid changing live symbol tables during callbacks. The exact behavior is not guaranteed by the API; the repository does not contain a verified compatibility guarantee for this mutation.
- The standard library's ordinary output uses `std::cout`; only debugger output has a stream setter.
- Public headers retain `Token` and `ReturnResult` for API signatures. Lexer tables and conversion/type helpers are declared in internal `include/detail/globals.hpp`, which is not installed with the package.

Example: [known_limitations.cpp](examples/known_limitations.cpp) checks host type parsing and preservation after a rejected update.
