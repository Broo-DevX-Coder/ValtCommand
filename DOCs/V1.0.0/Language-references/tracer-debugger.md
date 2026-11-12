# Debugger and tracer events

**Version:** 1.0.0

The runtime exposes events through `vltc::DevTools::EventsT` and the debugger API. `valtc --debug --logs` writes the event stream to standard error. The event categories follow these rules:

| Type | Use | Constraint |
|---|---|---|
| `Analysis` | Semantic checks inside `accept()` | Never use in `exec()` or for failures. |
| `Runtime` | The beginning of an AST node's `exec()` (`Executing …`) | One event per `exec()` entry. `ModuleNode` is the exception: it has no source statement line and emits no Runtime event. |
| `Info` | Beginning of statement execution and successful final results | Emit once for each such meaning, without duplicates. |
| `Debug` | Internal evaluation details, operands, operators, short-circuit decisions, comparisons, and the generic execution guard | The guard emits `AST node execution finished` for every node. |
| `Error` | Every unsuccessful `accept()` or `exec()` return | Emit immediately before returning, with a node-and-operation-specific message. A parent that propagates a child's failure emits its own contextual Error event and includes the child's message. |

For statement nodes, the order is execution guard, optional Info start event, Runtime event, then checks and work. Runtime carries the node's token line, falling back to its `END_token` line when the token line is unavailable. A loop emits one Runtime event when its `exec()` begins; its `Entering loop iteration` details are Debug events.

`Program execution completed successfully` is emitted by `ModuleNode::exec()` after all statements finish, so it is an Info success event. It is separate from the generic Debug event emitted by `ASTNodeExecCompletionGuard`.

The following excerpt was captured by running `valtc --debug --logs --eval 'SET i<int> = 1'`:

```text
[Analysis] line:1 -> Starting program semantic analysis
[Analysis] line:1 -> Analyzing program statement #1
[Analysis] line:1 -> Analyzing variable declaration 'i' with type 'int'
[Analysis] line:1 -> Analyzing numeric literal
[Analysis] line:1 -> Numeric literal validated successfully
[Analysis] line:1 -> Program semantic analysis completed successfully
[Info] line:1 -> Starting program execution
[Debug] line:1 -> Executing program statement #1
[Runtime] line:1 -> Executing variable declaration 'i'
[Runtime] line:1 -> Executing numeric literal
[Debug] line:1 -> Numeric literal evaluated successfully
[Debug] line:1 -> AST node execution finished
[Debug] line:1 -> Variable 'i' was initialized successfully
[Debug] line:1 -> AST node execution finished
[Info] line:1 -> Program execution completed successfully
[Debug] line:1 -> AST node execution finished
```

## Breakpoints

The CLI accepts `--break PATH:LINE[,LINE...]` or line-only `--break LINE[,LINE...]`, where the latter uses `--file`. Breakpoint matching normalizes both paths to absolute, lexically normalized paths. A line-only breakpoint with an empty path matches any file, so `valtc --debug --break 1 --eval '…'` can stop on an eval source line. The spelling `--break :1` is invalid because the parser treats it as an explicitly empty file path.

Adding the same file and line more than once registers one breakpoint; this was tested with the same `PATH:LINE` supplied twice.

CLI breakpoints remain active after a hit. A breakpoint in a loop body stops each time execution reaches that line after another line. In the documented two-iteration probe, the body-line breakpoint stopped twice. Several AST nodes may execute on the same source line; the debugger stops once for that source line even when multiple Runtime events occur there.

The event gate checks a Runtime event when either its line or its file differs from the last Runtime event. It then matches both file and line. This lets a breakpoint in a second file trigger even when that file's first Runtime event has the same line number as the preceding file's event. A breakpoint remains file-specific, so an event on the same numbered line in another file does not match it. The gate also means consecutive Runtime events on the same file and line do not cause repeated stops; reaching that line again after execution moves to another line can stop again.
