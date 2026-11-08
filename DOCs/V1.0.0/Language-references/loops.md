# Loops

**Version:** 1.0.0

The parser implements two loop forms: `WHILE` and `FOR`.

## Forms and syntax

`WHILE` evaluates its condition before each iteration. `FOR` requires an initializer, a condition, and an update expression, separated by semicolons. The body starts after `THEN`. Each loop must close with `END`.

```ebnf
while-statement ::= "WHILE" expression "THEN" { statement } "END"
for-statement ::= "FOR" expression ";" expression ";" expression "THEN" { statement } "END"
```

All three `FOR` header parts are parsed as ordinary expressions. For example, the initializer can declare the loop variable with `SET i<int> = 0`. The parser requires both semicolons and all three expressions. There is no other loop form in the parser.

- [WHILE example](examples/loops-while.vlt) · [actual output](examples/loops-while.expected)
- [FOR example](examples/loops-for-order.vlt) · [actual output](examples/loops-for-order.expected)

## FOR execution order

The interpreter evaluates the initializer once, checks the condition, runs the body when it is true, evaluates the update, then checks the condition again. The observed trace is `init`, `condition`, `body`, `step`, repeating until the condition is false. When the first condition is false, the body and update are skipped; the initializer has already run.

- [Order trace](examples/loops-for-order.vlt) · [actual output](examples/loops-for-order.expected)
- [Initially false condition](examples/loops-for-false-condition.vlt) · [actual output](examples/loops-for-false-condition.expected)

## Scopes and variables

`FOR` creates a loop scope used for its initializer, condition, and update. The loop body runs in a child scope that is created anew for each iteration. `WHILE` evaluates its condition in the surrounding scope and creates a child scope for each body iteration. A typed declaration in either body can shadow an outer variable; both tested examples print the inner value twice and then the unchanged outer value (`50`, `50`, `7`).

The `FOR` loop scope ends with the loop, so a header variable is not available afterward. The failing example attempts to read `i` after the loop and records the interpreter's `NameError`. See [FOR scope shadowing](examples/loops-scope-shadow.vlt) ([output](examples/loops-scope-shadow.expected)), [WHILE scope shadowing](examples/loops-while-scope-shadow.vlt) ([output](examples/loops-while-scope-shadow.expected)), and [the header variable after the loop](examples/loops-for-variable-after.vlt) ([error output](examples/loops-for-variable-after.expected)).

## Conditions and errors

Both loop conditions must have static type `bool`. A non-boolean `WHILE` condition reports `TypeError: expected 'bool', got 'int'` with `Condition of while loop must be boolean`; a non-boolean `FOR` condition reports the same type mismatch with `Condition of for loop must be boolean`. These checks happen during semantic analysis, before the loop executes.

The parser also requires `THEN` after the condition. The examples record the tested diagnostics:

- [WHILE condition type error](examples/loops-while-condition-type-error.vlt) · [error output](examples/loops-while-condition-type-error.expected)
- [FOR condition type error](examples/loops-for-condition-error.vlt) · [error output](examples/loops-for-condition-error.expected)
- [Missing `THEN`](examples/loops-while-missing-then.vlt) · [error output](examples/loops-while-missing-then.expected)

If `END` is missing, parsing returns a `SyntaxError` with the closing construct and the source location of the insertion point.

- [Missing `WHILE` terminator](examples/loops-missing-end.vlt) · [error output](examples/loops-missing-end.expected)
- [Missing `FOR` terminator](examples/loops-for-missing-end.vlt) · [error output](examples/loops-for-missing-end.expected)

## BREAK and CONTINUE

Inside a loop, `BREAK` exits the innermost active loop. `CONTINUE` skips the rest of the current body. In a `FOR` loop, execution then runs the update expression and reevaluates the condition; in a `WHILE` loop, it reevaluates the condition. The `FOR` example skips printing `1`, prints `0`, `2`, and `3`, and exits at `4`; the `WHILE` example skips `2`, prints `1` and `3`, then exits at `4`. The nested-loop example uses `CONTINUE` and `BREAK` in the inner loop; each outer iteration continues after the inner loop ends.

Use outside a loop is described in [Known limitations](limitations.md#known-limitations).

- [Single-loop control](examples/loops-break-continue.vlt) · [actual output](examples/loops-break-continue.expected)
- [WHILE control](examples/loops-while-break-continue.vlt) · [actual output](examples/loops-while-break-continue.expected)
- [Nested-loop control](examples/loops-nested-control.vlt) · [actual output](examples/loops-nested-control.expected)

## Loops and functions

A function body can contain a loop, and `BREAK`/`CONTINUE` inside that loop are handled by the loop before control returns to the function. A loop body can also call a function on each iteration. Both behaviors were run:

- [Loop inside a function](examples/loops-in-function.vlt) · [actual output](examples/loops-in-function.expected)
- [Function called by a loop](examples/loops-call-function.vlt) · [actual output](examples/loops-call-function.expected)
- `BREAK` and `CONTINUE` in a called function with no loop are ignored by that function body; they do not stop or skip the caller's loop. This resembles the current behavior outside a loop; see [Known limitations](limitations.md#known-limitations). [Example](examples/loops-break-in-called-function.vlt) · [actual output](examples/loops-break-in-called-function.expected)

## Iteration limits

The loop execution code contains no fixed iteration ceiling. A finite `WHILE` loop completed 10,000 iterations in the test below. A truly non-terminating loop was not run.

- [10,000-iteration check](examples/loops-iteration-count.vlt) · [actual output](examples/loops-iteration-count.expected)

