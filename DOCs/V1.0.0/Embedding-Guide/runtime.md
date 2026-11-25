# RunTime reference

`vltc::Runtime::RunTime` owns three global scopes, host native registrations and, optionally, a debugger. Runtime fields are private; hosts control the instance through the public members below. Calls from native/debugger callbacks are synchronous with script execution. Do not reset or re-enter the same runtime while an AST operation is using its scopes: replacing a scope can invalidate pointers in active frames.

## Construction and lifetime

### `RunTime(bool DebugMode = false)`

Creates scopes, registers built-in libraries, and optionally creates a debugger. Example: `vltc::Runtime::RunTime runtime;`. Scope state starts empty apart from built-ins and host registrations. Debugger-related changes require `true` at construction.

### `explicit RunTime(const std::string& code, bool DebugMode = false)`

Constructs the runtime with source saved for the library's compatibility path, then initializes it. Prefer constructing without code and calling `run()` or `run_file()`. Example: `RunTime runtime(source);`. The code is private state; use `run()` to execute new source.

### `~RunTime()`

Destroys scopes, debugger, and stored native registrations with the runtime. All pointers into its scopes and debugger become invalid. Example: normal stack lifetime, `RunTime runtime;`.

### `void Initialyze(bool DebugMode = false)`

Recreates all scopes and initializes built-in libraries. `DebugMode=true` creates a debugger; `false` does not create one on an uninitialized instance, but does not remove an already existing debugger. The header's spelling is `Initialyze`. Example: call the constructor instead. Calling it after script state has been added replaces the scopes and discards that state; use `reset()` when you want its documented reset behavior. It is not safe during execution or from a callback.

### `DevTools::Debugger* GetDebugger()`

Returns the debugger pointer or `nullptr` when debugging was disabled. Example: `if (auto* d = runtime.GetDebugger()) (void)d->get_all_events_strs();`. The pointer is owned by `RunTime`; do not delete it or retain it after runtime destruction. Calling debugger methods affects the active execution synchronously.

## Debugger stream access

### `void set_debugger_ostream(std::ostream& output)`

Changes the debugger's output stream when a debugger exists; otherwise it does nothing. Example: `runtime.set_debugger_ostream(log);`. The stream must outlive its use. The change affects subsequent debugger writes; it does not redirect ordinary script `print`/`println` output, which uses `std::cout`.

### `const std::ostream* get_debugger_ostream() const`

Returns the current debugger stream or `nullptr` when debugging is disabled. Example: `auto* stream = runtime.get_debugger_ostream();`. The pointer is non-owning. Reading it during a callback is safe; changing stream lifetime while the debugger uses it is not.

## Execution

### `RunResult run(const std::string& code, ScopeMode mode = ScopeMode::Persistent)`

Analyzes and executes source text. Persistent mode shares global script state between calls; Fresh mode uses temporary scopes for that call. Example: `auto r = runtime.run("SET n<int> = 4");`. Errors are returned in `RunResult`. Re-entering this method on the same runtime from an active callback is not a supported operation and can replace or reuse state being executed.

### `RunResult run_file(const std::string& path, ScopeMode mode = ScopeMode::Persistent)`

Reads a file and uses the same pipeline as `run()`. File-open failure returns `ErrorStage::IO`. Example: `auto r = runtime.run_file("script.vlt");`. The source path is attached to diagnostics. Avoid re-entry during active execution for the same reasons as `run()`.

## State management

### `void reset()`

Clears saved source/last-result fields, recreates all three global scopes, and reinstalls registered host native functions and constants. This removes script-created global variables and user-defined functions while preserving host registrations. Example: `runtime.reset(); assert(!runtime.has_variable("script_value"));`. It deletes all user entries at once; it does not expose single-entry deletion. It does not recreate the separately owned debugger, so debugger history and breakpoint configuration are retained. Do not call during an active run or from a native/debugger callback: scope pointers held by active code can become invalid.

### `RunResult set_variable(const std::string& name, const std::string& value, const std::string& type, bool is_constant = false)`

Parses text as `int`, `float`, `bool`, or `str` and creates/replaces the global variable in the run, run-analysis, and semantic scopes. Empty names, unknown types and invalid text return `ErrorStage::Host`. Registered native constant names are rejected. Example: `runtime.set_variable("limit", "8", "int");`. This may replace a script variable, including a script constant. Calling it while script code uses the entry can invalidate pointers and is unsupported.

### `RunResult get_variable(const std::string& name)`

Reads a global variable and returns its display text/type; missing names return a host error. Example: `auto r = runtime.get_variable("limit");`. Read access does not mutate execution state.

### `bool has_variable(const std::string& name)`

Checks whether a global variable exists. Example: `if (runtime.has_variable("limit")) { /* ... */ }`. This is a read-only query.

## Native registrations

### `void add_external_function(ExternalFuncType function, const std::string& name, const std::string& return_type, std::unordered_map<std::string, ExternalFunctionArgument> arguments, bool is_any = false)`

Registers a host callback with a script-visible name and signature. It is installed into the active scopes and retained across `reset()` and Fresh runs. Example: `runtime.add_external_function(callback, "host_call", "int", {});`. Re-registering a name replaces the scope registration. Register before executing source; changing registrations during a callback can make analyzed signatures and active function pointers disagree.

### `bool add_external_constant(const std::string& name, const Value& value)`

Registers an immutable script-visible constant; its language type comes from the `Value` alternative. It returns false for an empty name or void value and survives `reset()` and Fresh runs. Example: `runtime.add_external_constant("ANSWER", int64_t{42});`. Adding an existing name replaces that native registration and scope entry. Register before execution; changing a constant during active execution can invalidate entries.
