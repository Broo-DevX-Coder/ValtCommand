# Variables

**Version:** 1.0.0

This page describes variable syntax and scope behavior observed in the current interpreter. The examples are `.vlt` programs; each has an adjacent `.expected` file captured from a run.

## Declare, read, and assign

A new mutable variable is declared with `SET`, an explicit type, `=`, and an initial expression. Constants use `SET_CONST` with the same typed initializer form:

```text
SET x<int> = 5
SET_CONST limit<int> = 10
```

The type and initial value are required for either declaration. `SET x = 5` has no declaration form: the parser treats it as an assignment to an existing variable, and an unknown `x` produces `NameError: 'x' is not defined`. A typed declaration with no `=` and value fails during parsing. See [untyped `SET`](examples/variables-untyped-declaration.vlt) ([output](examples/variables-untyped-declaration.expected)) and [missing initializer](examples/variables-missing-initializer.vlt) ([output](examples/variables-missing-initializer.expected)). The tested [constant declaration](examples/variables-constant.vlt) prints `5`; attempting `SET limit = 11` afterward produces `ImmutableError` in [this example](examples/variables-assign-user-constant.vlt) ([actual error](examples/variables-assign-user-constant.expected)).

Use `GET` to read a variable. A bare identifier is not a variable-read expression; `x` where an expression is expected produces the tested `SyntaxError: unexpected token 'x'`. Reading an undeclared name with `GET` produces `NameError: 'missing' is not defined` and the formatter adds the comment `Unknown variable`.

- [Declaration, read, and reassignment](examples/variables-declare-reassign.vlt) ([actual output](examples/variables-declare-reassign.expected))
- [Bare identifier without `GET`](examples/variables-bare-name.vlt) ([actual error](examples/variables-bare-name.expected))
- [Read an undeclared variable](examples/variables-undeclared.vlt) ([actual error](examples/variables-undeclared.expected))

`SET x = expression` assigns to the existing binding and does not repeat its type. The declared type does not change on assignment. In the example above, assigning `2.5` and `-2.9` to an `int` variable leaves it as `int` and prints `2` and `-2`; current silent float-to-int conversion is listed in [Known limitations](limitations.md#known-limitations). Declaring the same name again in the same scope fails with `VariableError`, even if the second declaration names a different type; see [same-scope redeclaration](examples/variables-redeclaration.vlt) ([actual error](examples/variables-redeclaration.expected)). A nonnumeric incompatible assignment is covered in [Types and Values](types-and-values.md#numeric-conversion-and-arithmetic-results).

## Names

Variable names use the identifier rules in [Lexical Structure](lexical-structure.md#5-identifiers): an ASCII letter or `_` first, followed by ASCII letters, digits, or `_`. Matching is case-sensitive, so `item` and `Item` are distinct names. Exact uppercase keywords such as `IF` are reserved tokens and cannot occupy the identifier position after `SET`; the tested declaration fails with `SyntaxError: unexpected token 'IF'`. The lowercase name `if` is accepted. The lexer has no explicit maximum identifier length; a long identifier is included in the passing [name example](examples/variables-names.vlt) ([output](examples/variables-names.expected)).

`Math` and `STDIO` are C++ library namespaces, not reserved source-language variable names in this build. Both can be declared as variables, as shown in [the name example](examples/variables-library-names.vlt) ([output](examples/variables-library-names.expected)). A variable named `STDIO` does not prevent calls to standard I/O functions: after declaring `STDIO`, the [tested example](examples/variables-shadow-stdio.vlt) calls `println` successfully ([actual output](examples/variables-shadow-stdio.expected)).

## Scopes, shadowing, and lifetime

Top-level declarations belong to the runtime's global scope. An `IF` creates a child scope for its selected branch; a function call creates a call scope. A declaration in an inner scope can reuse a name from an outer scope and shadows that binding while the inner scope is active. When the `IF` finishes, the outer value is visible again: the [shadowing example](examples/variables-if-shadow.vlt) prints `50` and then `7`. A new variable declared in an `IF` branch cannot be read after the `IF`; the attempted read reports `NameError` in [this example](examples/variables-if-local-scope.vlt) ([actual error](examples/variables-if-local-scope.expected)).

The current loop scope rules are described in [Loops](loops.md#scopes-and-variables). Function-call scopes and lookup from caller scopes are described in [Functions](functions.md#scope-and-definition-order).

An untyped assignment searches the active scope and its parents. If it finds a mutable outer binding, it updates that binding instead of creating a local variable. The tested `IF` assignment changes the global value to `50`; the function example reads the global value, increments it, and prints `8` from both the call result and the global binding afterward. See [assignment from `IF`](examples/variables-if-outer-assignment.vlt) ([output](examples/variables-if-outer-assignment.expected)) and [assignment from a function](examples/variables-function-outer-assignment.vlt) ([output](examples/variables-function-outer-assignment.expected)).

Function-local variables are visible inside the call, as shown by [the local-variable example](examples/variables-function-local-visible.vlt) ([actual output](examples/variables-function-local-visible.expected)). A top-level reference to a function-local name fails semantic analysis with `NameError`; the interpreter rejects the complete program before executing it, as recorded in [the local-scope error example](examples/variables-function-local-scope.vlt) ([actual error](examples/variables-function-local-scope.expected)). In the implementation, each function call and each `IF` execution owns a child `Scope` object whose entries are destroyed with that scope. No variable-binding leak into the enclosing scope was observed; memory-leak behavior is outside what these language examples test.

## Built-in names and inspection

The runtime registers built-in constants and functions in the global scope. The names `Math` and `STDIO` themselves can be used as variable names (see [the test](examples/variables-library-names.vlt)); individual built-in names such as `PI` are already constant variables. `SET PI = 3` currently fails with `ImmutableError: Cannot modify constant variable 'PI'` ([tested example](examples/variables-pi-assignment.vlt), [actual error](examples/variables-pi-assignment.expected)).

`__INSPECT_VARS__` returns a printable table of variables visible from the active scope, and `__INSPECT_SCOPE__` returns summary information for that scope. The variable table includes parent-scope entries before entries from the active scope. Within one scope, the implementation iterates an `unordered_map`, so the row order is not stable. If an inner variable shadows an outer variable, the printed table currently contains a row for each binding with the same name; this behavior is listed as a known limitation. A direct interpreter probe printed two `value` rows, one from each scope. The [shadow-inspection example](examples/variables-inspect-shadowing.vlt) ([actual output](examples/variables-inspect-shadowing.expected)) calls the inspector but discards its order-sensitive table, so its expected output does not compare row order. The [inspection example](examples/variables-inspect.vlt) ([actual output](examples/variables-inspect.expected)) also calls it and prints the scope summary without snapshotting row order.

These inspection details describe the current implementation and may change in a later version.
