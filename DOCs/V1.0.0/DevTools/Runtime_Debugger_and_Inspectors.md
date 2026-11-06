# Runtime Debugger, Events, Breakpoints, and Inspectors

This guide documents the debugger and inspection facilities exposed by the ValtCommand runtime. It covers the event tracker, breakpoint and pause controls, the host API, the built-in inspectors, and the script-callable inspector functions.

## Enable the debugger

Create the runtime with debug mode enabled. The debugger is created during `Initialyze()` only when `DebugMode` is `true`.

```cpp
#include "API/Runtime.hpp"

Runtime::RunTime runtime(true);
```

When debug mode is disabled, `GetDebugger()` returns `nullptr`. Runtime wrappers that mutate debugger state do nothing, string getters return an empty string, and `get_ostream()` returns `nullptr`.

## Event tracker

### Event data

Each `DevTools::event` contains:

| Field | Meaning |
| --- | --- |
| `type` | Event category (`Info`, `Analysis`, `Runtime`, `Debug`, `Warning`, or `Error`) |
| `message` | Description emitted by the runtime or AST node |
| `line` | Source line associated with the event |
| `curent_scope` | Scope active at that point, or `nullptr` if unavailable |
| `file` | Source file path; string-run code may have an empty path |

`event.get_str()` formats an event as `[Category] line:N -> message`. The formatted string does not include the file path or scope pointer; a host callback can inspect those fields directly.

AST node event helpers fill the scope and path from `NodesContext`. Each AST node `exec()` also has a completion guard, which emits a `Runtime` event using that node's `END_token` line, current scope, and file when the function exits. This includes early-return paths.

### Event flow

`Debugger::emit()` processes an event in this order:

1. It checks breakpoints for non-`Analysis` events when the line differs from the last tracked line.
2. It appends the event to the debugger's event list.
3. It invokes the configured tracker callback.

Analysis events are still recorded and sent to the callback; they simply do not trigger breakpoints. For a given source line, only the first non-analysis event after a line change runs the breakpoint check.

The default tracker callback prints formatted events. Setting a custom callback replaces that default:

```cpp
runtime.set_emit_event_slot([](DevTools::event event) {
    std::cout << event.file << ':' << event.line
              << " [" << DevTools::event_type_to_string(event.type) << "] "
              << event.message << '\n';
});
```

The callback receives the full event by value, including `file` and `curent_scope`. Treat the scope pointer as borrowed: use it while the callback is running, and do not retain it beyond the associated execution scope's lifetime.

### Runtime tracker host methods

`Runtime::RunTime` forwards these controls to its debugger:

| Runtime method | Purpose |
| --- | --- |
| `emit_event(event)` | Manually emit an event |
| `set_emit_event_slot(slot)` | Replace the tracker callback |
| `get_next_event_str()` | Retrieve the next formatted event |
| `get_all_events_strs()` | Retrieve all formatted events |
| `set_ostream(stream)` / `get_ostream()` | Set or retrieve the debugger's configured stream |

Events remain in the debugger's event list for its lifetime; the Runtime currently exposes no method to clear the list. `get_all_events_strs()` does not change the read position. `get_next_event_str()` advances its read position before returning; with the current initial position, the first call returns the second stored event when at least two events exist.

The current default print callback writes directly to `std::cout`, even when `set_ostream()` has configured a different stream. Use `set_emit_event_slot()` when the host needs to redirect tracker output.

## Breakpoints and debugger controls

### Add breakpoints

Breakpoints match a 1-based source line and optionally a file path:

```cpp
runtime.add_break_points({12, 20}, "scripts/example.vlt");
```

An empty path keeps line-only matching and matches events from any file:

```cpp
runtime.add_break_points({12});
```

`check_break_point(line, scope, FilePath)` performs a direct check. Normally `Debugger::emit()` performs checks automatically, passing the event's line, scope pointer, and file path.

A matched permanent breakpoint remains registered for later visits to that line. A breakpoint with a non-empty path only matches the same path string supplied by the event; the debugger does not normalize or canonicalize paths.

### Pause and resume

`set_stop_slot()` replaces the default interactive pause handler. The callback receives the debugger and active scope:

```cpp
runtime.set_stop_slot(
    [](DevTools::Debugger& debugger, Scopes::Scope* scope) {
        if (scope) {
            auto variables = DevTools::VariablesInspect(scope);
            std::cout << variables.printable;
        }
        debugger.next_line();
    }
);
```

The default handler reads commands from standard input:

| Command | Alias | Action |
| --- | --- | --- |
| `continue` | `c` | Return from the pause handler and continue execution |
| `nextl` | `n` | Add a temporary breakpoint for the next source line, then continue |
| `exit` | `e` | Call `Debugger::exit()` and return from the pause handler |
| `show vars` | `sv` | Print the variables inspector for the active scope |
| `show functions` | `sf` | Print the functions inspector for the active scope |
| `show scope info` | `sc` | Print the scope inspector for the active scope |

The host can call the same controls through `RunTime`: `continue_exec()`, `next_line()`, and `exit_debugging()`. The debugger's `continue_exec()` currently has an empty implementation; resumption occurs when the synchronous stop callback returns. `exit()` currently sets an internal flag, but the execution path does not check that flag to abort the Runtime. Thus, `exit` presently leaves the pause handler rather than reliably terminating script execution.

`next_line()` uses the most recently emitted event's file path and line plus one. Its breakpoint is temporary and is removed after it is used.

## Inspectors

The inspector functions are in `API/DevTools/Inspectures.hpp`. Each returns a `DevTools::InspecturesReturn` containing both `printable` text and a `nlohmann::json` value.

### `ScopeInspect(scope)`

Reports the scope ID, optional parent scope ID, depth, number of visible variables, and number of visible functions. The JSON keys are `scope_id`, optional `parent_id`, `depth`, `visible_vars`, and `visible_funcs`.

The current implementation expects a non-null scope pointer.

### `VariablesInspect(scope)`

Returns a formatted table of visible variables and JSON keyed by variable name. Each variable entry includes `scope_id`, `constant`, `data_type`, and `value`. Values are converted from the Runtime's `Value` variant to JSON scalars or `null`.

Passing a null scope returns an error string and an empty JSON object. Scope tables include entries from parent scopes as well as the current scope.

### `FunctionsInspect(scope)`

Returns a formatted table of visible functions and JSON keyed by function name. Each function entry includes `scope_id`, `return_type`, `function_type` (`Inside` or `External`), `any_arguments`, and an `arguments` object. Each argument records its type, whether it is required, whether it accepts any type, and its default value.

Passing a null scope returns an error string and an empty JSON object. Function tables include functions from parent scopes.

Host-side use:

```cpp
#include "API/DevTools/Inspectures.hpp"

if (auto* debugger = runtime.GetDebugger()) {
    // A scope pointer is supplied to the pause callback.
    auto inspection = DevTools::ScopeInspect(scope);
    std::cout << inspection.printable;
    nlohmann::json data = inspection.JSON;
}
```

## Script-callable inspector functions

The DevTools standard library registers three native functions that inspect the scope of the current script call and return the printable text as a string:

| Script function | Inspector used |
| --- | --- |
| `__INSPECT_SCOPE__()` | `ScopeInspect()` |
| `__INSPECT_VARS__()` | `VariablesInspect()` |
| `__INSPECT_FUNCS__()` | `FunctionsInspect()` |

These functions are initialized with the other standard-library functions by the Runtime. They return an empty string if no parent scope is supplied to the native call.

## Host API summary

```cpp
Runtime::RunTime runtime(true);

runtime.set_emit_event_slot([](DevTools::event event) {
    // Consume event.type, event.message, event.line, event.file,
    // and event.curent_scope here.
});

runtime.add_break_points({12}, "scripts/example.vlt");
runtime.set_stop_slot([](DevTools::Debugger& debugger, Scopes::Scope* scope) {
    if (scope) {
        const auto vars = DevTools::VariablesInspect(scope);
        std::cout << vars.printable;
    }
    debugger.next_line();
});

const auto result = runtime.run_file("scripts/example.vlt");
const std::string events = runtime.get_all_events_strs();
```
